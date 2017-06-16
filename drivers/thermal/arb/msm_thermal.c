/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
 /* ARB THERMAL CONFIGURARTION */

#define pr_fmt(fmt) "%s:%s " fmt, KBUILD_MODNAME, __func__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/msm_tsens.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/msm_tsens.h>
#include <linux/msm_thermal.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/thermal.h>
#include <linux/regulator/rpm-smd-regulator.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/msm_thermal_ioctl.h>
#include <soc/qcom/rpm-smd.h>
#include <soc/qcom/scm.h>
#include <linux/sched/rt.h>
#include <linux/ratelimit.h>

#define CREATE_TRACE_POINTS
#define TRACE_MSM_THERMAL
#include <trace/trace_thermal.h>

#define MAX_CURRENT_UA 100000
#define MAX_RAILS 5
#define MAX_THRESHOLD 2
#define MONITOR_ALL_TSENS -1
#define TSENS_NAME_MAX 20
#define TSENS_NAME_FORMAT "tsens_tz_sensor%d"
#define THERM_SECURE_BITE_CMD 8
#define SENSOR_SCALING_FACTOR 1
#define CPU_DEVICE "cpu%d"

/* Throttle CPU when it reaches a certain temp*/
unsigned int temp_threshold = 50;
module_param(temp_threshold, int, 0644);

static struct thermal_info {
	uint32_t cpuinfo_max_freq;
	uint32_t limited_max_freq;
	unsigned int safe_diff;
	bool throttling;
	bool pending_change;
} info = {
	.cpuinfo_max_freq = LONG_MAX,
	.limited_max_freq = LONG_MAX,
	.safe_diff = 5,
	.throttling = false,
	.pending_change = false,
};

/* Throttle Freq in MHz */
unsigned int FREQ_ZONEH	= 200000;
module_param(FREQ_ZONEH, int, 0644);

unsigned int FREQ_ZONEG	= 345600;
module_param(FREQ_ZONEG, int, 0644);

unsigned int FREQ_ZONEF	= 533333;
module_param(FREQ_ZONEF, int, 0644);

unsigned int FREQ_ZONEE	= 800000;
module_param(FREQ_ZONEE, int, 0644);

unsigned int FREQ_ZONED	= 1113600;
module_param(FREQ_ZONED, int, 0644);

unsigned int FREQ_ZONEC	= 1344000;
module_param(FREQ_ZONEC, int, 0644);

unsigned int FREQ_ZONEB	= 1459200;
module_param(FREQ_ZONEB, int, 0644);

unsigned int FREQ_ZONEA	= 1574000;
module_param(FREQ_ZONEA, int, 0644);
 
unsigned int FREQ_ZONE = 1708000;
module_param(FREQ_ZONE, int, 0644);


/* Temp Diferrence */
unsigned int temp_step = 3;
module_param(temp_step, int, 0644);

/* Core control temp */
int corecontrol = 60;

/* CPU Hotplugging Switch */
unsigned int temp_safety = 1;
module_param(temp_safety, int, 0644);

/* Extras */
unsigned int enabled = 0;
module_param(enabled, int, 0664);

static struct msm_thermal_data msm_thermal_info;
static struct delayed_work check_temp_work;
static struct workqueue_struct *thermal_wq;

static void cpu_offline_wrapper(int cpu)
{
        if (cpu_online(cpu))
			cpu_down(cpu);
}

static void __ref cpu_online_wrapper(int cpu)
{
        if (!cpu_online(cpu))
			cpu_up(cpu);
}

static int msm_thermal_cpufreq_callback(struct notifier_block *nfb,
		unsigned long event, void *data)
{
	struct cpufreq_policy *policy = data;

	if (event != CPUFREQ_ADJUST && !info.pending_change)
		return 0;

	cpufreq_verify_within_limits(policy, policy->cpuinfo.min_freq,
		info.limited_max_freq);

	return 0;
}

static struct notifier_block msm_thermal_cpufreq_notifier = {
	.notifier_call = msm_thermal_cpufreq_callback,
};

static void limit_cpu_freqs(uint32_t max_freq)
{
	unsigned int cpu;

	if (info.limited_max_freq == max_freq)
		return;

	info.limited_max_freq = max_freq;
	info.pending_change = true;
	pr_info_ratelimited("%s: Setting cpu max frequency to %u\n",
	KBUILD_MODNAME, max_freq);

	get_online_cpus();
	for_each_online_cpu(cpu) {
		cpufreq_update_policy(cpu);
		pr_info("%s: Setting cpu%d max frequency to %d\n",
				KBUILD_MODNAME, cpu, info.limited_max_freq);
	}
	put_online_cpus();

	info.pending_change = false;
}

static void check_temp(struct work_struct *work)
{
	struct tsens_device tsens_dev;
	uint32_t freq = 0;
	long temp = 0;

	tsens_dev.sensor_num = msm_thermal_info.sensor_id;
	tsens_get_temp(&tsens_dev, &temp);

	if (info.throttling) {
		if (temp < (temp_threshold - info.safe_diff)) {
			limit_cpu_freqs(info.cpuinfo_max_freq);
			info.throttling = false;
			goto reschedule;
		}
	}

	if (temp >= temp_threshold + (temp_step * 7))
		freq = FREQ_ZONEH;
	else if (temp >= temp_threshold + (temp_step * 6))
		freq = FREQ_ZONEG;
	else if (temp >= temp_threshold + (temp_step * 5))
		freq = FREQ_ZONEF;
	else if (temp >= temp_threshold + (temp_step * 4))
		freq = FREQ_ZONEE;
	else if (temp >= temp_threshold + (temp_step * 3))
		freq = FREQ_ZONED;
	else if (temp >= temp_threshold + (temp_step * 2))
		freq = FREQ_ZONEC;
	else if (temp >= temp_threshold + temp_step)
		freq = FREQ_ZONEB;
	else if (temp >= temp_threshold)
		freq = FREQ_ZONEA;
	else if (temp < temp_threshold)
		freq = FREQ_ZONE;

	if (freq) {
		limit_cpu_freqs(freq);

		if (!info.throttling)
			info.throttling = true;
	}

/* CPU UNplugging */
if(temp_safety==1){
	if (temp >= (corecontrol)){
	    cpu_offline_wrapper(1);
		cpu_offline_wrapper(2);
		cpu_offline_wrapper(3);}
	if (temp >= (corecontrol + 6)){
	    cpu_offline_wrapper(4);
		cpu_offline_wrapper(5);}
}


/* CPU Plugging */
if(temp_safety==1){
	if (temp < (corecontrol)){
	    cpu_online_wrapper(1);
		cpu_online_wrapper(2);
		cpu_online_wrapper(3);}
    if (temp < (corecontrol + 6)){
	    cpu_online_wrapper(4);
		cpu_online_wrapper(5);
		cpu_online_wrapper(6);
		cpu_online_wrapper(7);}
}


/* Quad core Mode */
if(enabled==1){
		cpu_online_wrapper(1);
		if(temp_safety==1){
	    if (temp > (corecontrol)){
		cpu_offline_wrapper(1);}}
	    cpu_offline_wrapper(2);
		cpu_offline_wrapper(3);
	    cpu_offline_wrapper(6);
		cpu_offline_wrapper(7);
		cpu_online_wrapper(4);
		cpu_online_wrapper(5);}


reschedule:
	queue_delayed_work(system_power_efficient_wq, &check_temp_work, msecs_to_jiffies(250));
}

static int msm_thermal_dev_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct device_node *node = pdev->dev.of_node;
	struct msm_thermal_data data;

	memset(&data, 0, sizeof(struct msm_thermal_data));

	ret = of_property_read_u32(node, "qcom,sensor-id", &data.sensor_id);
	if (ret)
		return ret;

	WARN_ON(data.sensor_id >= TSENS_MAX_SENSORS);

        memcpy(&msm_thermal_info, &data, sizeof(struct msm_thermal_data));

	ret = cpufreq_register_notifier(&msm_thermal_cpufreq_notifier,
		CPUFREQ_POLICY_NOTIFIER);
	if (ret)
		pr_err("thermals: well, if this fails here, we're fucked\n");

	thermal_wq = alloc_workqueue("thermal_wq", WQ_HIGHPRI, 0);
	if (!thermal_wq) {
		pr_err("thermals: don't worry, if this fails we're also bananas\n");
		goto err;
	}

	INIT_DELAYED_WORK(&check_temp_work, check_temp);
	queue_delayed_work(system_power_efficient_wq, &check_temp_work, 5);

err:
	return ret;
}

static int msm_thermal_dev_remove(struct platform_device *pdev)
{
	cancel_delayed_work_sync(&check_temp_work);
	destroy_workqueue(thermal_wq);
	cpufreq_unregister_notifier(&msm_thermal_cpufreq_notifier,
                        CPUFREQ_POLICY_NOTIFIER);
	return 0;
}

static struct of_device_id msm_thermal_match_table[] = {
	{.compatible = "qcom,msm-thermal"},
	{},
};

static struct platform_driver msm_thermal_device_driver = {
	.probe = msm_thermal_dev_probe,
	.remove = msm_thermal_dev_remove,
	.driver = {
		.name = "msm-thermal",
		.owner = THIS_MODULE,
		.of_match_table = msm_thermal_match_table,
	},
};

static int __init msm_thermal_device_init(void)
{
	return platform_driver_register(&msm_thermal_device_driver);
}

static void __exit msm_thermal_device_exit(void)
{
	platform_driver_unregister(&msm_thermal_device_driver);
}

arch_initcall(msm_thermal_device_init);
module_exit(msm_thermal_device_exit);
