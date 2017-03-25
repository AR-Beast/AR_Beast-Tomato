/*
 * WCD93xx Sound Engine
 *
 * Copyright (C) 2013, Paul Reioux
 * Copyright (C) 2016, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kallsyms.h>
#include "msm8x16_wcd_registers.h"


#define SOUND_CONTROL_MAJOR_VERSION	4
#define SOUND_CONTROL_MINOR_VERSION	0

extern struct snd_soc_codec *snd_engine_codec_ptr;
//extern int wcd9xxx_hw_revision;

int snd_ctrl_enabled = 1;
static int snd_ctrl_locked = 2;
static int snd_rec_ctrl_locked = 0;
//static int actual_pa_gain = 36;

unsigned int msm8x16_wcd_read(struct snd_soc_codec *codec, unsigned int reg);
int msm8x16_wcd_write(struct snd_soc_codec *codec, unsigned int reg,
		unsigned int value);

#define REG_SZ	25
static unsigned int cached_regs[] = {6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
			    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			    0, 0, 0, 0, 0 };

static unsigned int *cache_select(unsigned int reg)
{
	unsigned int *out = NULL;

	switch (reg) {
		case MSM8X16_WCD_A_CDC_RX1_VOL_CTL_B2_CTL:
			out = &cached_regs[4];
			break;
		case MSM8X16_WCD_A_CDC_RX2_VOL_CTL_B2_CTL:
			out = &cached_regs[5];
			break;
		case MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B2_CTL:
			out = &cached_regs[6];
			break;
		case MSM8X16_WCD_A_CDC_TX1_VOL_CTL_GAIN:
			out = &cached_regs[11];
			break;
		case MSM8X16_WCD_A_CDC_TX2_VOL_CTL_GAIN:
			out = &cached_regs[12];
			break;
		case MSM8X16_WCD_A_CDC_TX3_VOL_CTL_GAIN:
			out = &cached_regs[13];
			break;
		case MSM8X16_WCD_A_CDC_TX4_VOL_CTL_GAIN:
			out = &cached_regs[14];
			break;
	}

	return out;
}

void snd_cache_write(unsigned int reg, unsigned int value)
{
	unsigned int *tmp = cache_select(reg);

	if (tmp != NULL)
		*tmp = value;
}
EXPORT_SYMBOL(snd_cache_write);

unsigned int snd_cache_read(unsigned int reg)
{
	if (cache_select(reg) != NULL)
		return *cache_select(reg);
	else
		return -1;
}
EXPORT_SYMBOL(snd_cache_read);

int snd_reg_access(unsigned int reg)
{
	int ret = 1;

	switch (reg) {
		case MSM8X16_WCD_A_CDC_RX1_VOL_CTL_B2_CTL:
		case MSM8X16_WCD_A_CDC_RX2_VOL_CTL_B2_CTL:
		case MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B2_CTL:
			if (snd_ctrl_locked > 0)
				ret = 0;
			break;
		case MSM8X16_WCD_A_CDC_TX1_VOL_CTL_GAIN:
		case MSM8X16_WCD_A_CDC_TX2_VOL_CTL_GAIN:
		case MSM8X16_WCD_A_CDC_TX3_VOL_CTL_GAIN:
		case MSM8X16_WCD_A_CDC_TX4_VOL_CTL_GAIN:
			if (snd_rec_ctrl_locked > 0)
				ret = 0;
			break;
		default:
			break;
	}
	return ret;
}
EXPORT_SYMBOL(snd_reg_access);

static ssize_t cam_mic_gain_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n",
		msm8x16_wcd_read(snd_engine_codec_ptr,
			MSM8X16_WCD_A_CDC_TX1_VOL_CTL_GAIN));
}

static ssize_t cam_mic_gain_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int lval, chksum;

	sscanf(buf, "%u %u", &lval, &chksum);

	if (!snd_ctrl_enabled)
		return count;

	snd_ctrl_locked = 0;
	msm8x16_wcd_write(snd_engine_codec_ptr,
		MSM8X16_WCD_A_CDC_TX1_VOL_CTL_GAIN, lval);
	snd_ctrl_locked = 2;

	return count;
}

static ssize_t mic_gain_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n",
		msm8x16_wcd_read(snd_engine_codec_ptr,
			MSM8X16_WCD_A_CDC_TX2_VOL_CTL_GAIN));
}

static ssize_t mic_gain_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int lval, chksum;

	sscanf(buf, "%u %u", &lval, &chksum);

	if (!snd_ctrl_enabled)
		return count;

	snd_ctrl_locked = 0;
	msm8x16_wcd_write(snd_engine_codec_ptr,
		MSM8X16_WCD_A_CDC_TX2_VOL_CTL_GAIN, lval);
	snd_ctrl_locked = 2;

	return count;

}

static ssize_t speaker_gain_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u %u\n",
			msm8x16_wcd_read(snd_engine_codec_ptr,
				MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B2_CTL),
			msm8x16_wcd_read(snd_engine_codec_ptr,
				MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B2_CTL));
}

static ssize_t speaker_gain_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int lval, rval, chksum;

	sscanf(buf, "%u %u %u", &lval, &rval, &chksum);

	if (!snd_ctrl_enabled)
		return count;

	snd_ctrl_locked = 0;
	msm8x16_wcd_write(snd_engine_codec_ptr,
		MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B2_CTL, lval);
	msm8x16_wcd_write(snd_engine_codec_ptr,
		MSM8X16_WCD_A_CDC_RX3_VOL_CTL_B2_CTL, rval);
	snd_ctrl_locked = 2;

	return count;
}

static ssize_t headphone_gain_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u %u\n",
			msm8x16_wcd_read(snd_engine_codec_ptr,
				MSM8X16_WCD_A_CDC_RX1_VOL_CTL_B2_CTL),
			msm8x16_wcd_read(snd_engine_codec_ptr,
				MSM8X16_WCD_A_CDC_RX2_VOL_CTL_B2_CTL));
}

static ssize_t headphone_gain_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int lval, rval, chksum;

	sscanf(buf, "%u %u %u", &lval, &rval, &chksum);

	if (!snd_ctrl_enabled)
		return count;

	snd_ctrl_locked = 0;
	msm8x16_wcd_write(snd_engine_codec_ptr,
		MSM8X16_WCD_A_CDC_RX1_VOL_CTL_B2_CTL, lval);
	msm8x16_wcd_write(snd_engine_codec_ptr,
		MSM8X16_WCD_A_CDC_RX2_VOL_CTL_B2_CTL, rval);
	snd_ctrl_locked = 2;

	return count;
}

/*static ssize_t headphone_pa_gain_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u %u\n",
		actual_pa_gain, actual_pa_gain);
}

static ssize_t headphone_pa_gain_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int lval, rval, chksum;
	unsigned int gain, status;
	unsigned int out;

	sscanf(buf, "%u %u %u", &lval, &rval, &chksum);

	if (!snd_ctrl_enabled)
		return count;

	snd_ctrl_locked = 0;

	gain = msm8x16_wcd_read(snd_engine_codec_ptr, MSM8X16_WCD_A_RX_HPH_L_GAIN);
	out = (gain & 0xf0) | lval;
	msm8x16_wcd_write(snd_engine_codec_ptr, MSM8X16_WCD_A_RX_HPH_L_GAIN, out);

	actual_pa_gain = out;

	status = msm8x16_wcd_read(snd_engine_codec_ptr, MSM8X16_WCD_A_RX_HPH_L_STATUS);
	out = (status & 0x0f) | (lval << 4);
	msm8x16_wcd_write(snd_engine_codec_ptr, MSM8X16_WCD_A_RX_HPH_L_STATUS, out);

	gain = msm8x16_wcd_read(snd_engine_codec_ptr, MSM8X16_WCD_A_RX_HPH_R_GAIN);
	out = (gain & 0xf0) | rval;
	msm8x16_wcd_write(snd_engine_codec_ptr, MSM8X16_WCD_A_RX_HPH_R_GAIN, out);

	status = msm8x16_wcd_read(snd_engine_codec_ptr, MSM8X16_WCD_A_RX_HPH_R_STATUS);
	out = (status & 0x0f) | (rval << 4);
	msm8x16_wcd_write(snd_engine_codec_ptr, MSM8X16_WCD_A_RX_HPH_R_STATUS, out);

	snd_ctrl_locked = 2;

	return count;
}*/

static unsigned int selected_reg = 0xdeadbeef;

static ssize_t sound_reg_select_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	if (!snd_ctrl_enabled)
		return count;

	sscanf(buf, "%u", &selected_reg);

	return count;
}

static ssize_t sound_reg_read_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	if (selected_reg == 0xdeadbeef)
		return -1;
	else
		return sprintf(buf, "%u\n",
			msm8x16_wcd_read(snd_engine_codec_ptr, selected_reg));
}

static ssize_t sound_reg_write_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int out;

	sscanf(buf, "%u", &out);

	if (!snd_ctrl_enabled)
		return count;

	if (selected_reg != 0xdeadbeef)
		msm8x16_wcd_write(snd_engine_codec_ptr, selected_reg, out);

	return count;
}

/*static ssize_t sound_control_hw_revision_show (struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "hw_revision: %i\n", wcd9xxx_hw_revision);
}*/

static ssize_t sound_control_version_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "version: %u.%u\n",
			SOUND_CONTROL_MAJOR_VERSION,
			SOUND_CONTROL_MINOR_VERSION);
}

static ssize_t sound_control_rec_locked_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int inp;

	sscanf(buf, "%d", &inp);

	snd_rec_ctrl_locked = inp;

	return count;
}

static ssize_t sound_control_rec_locked_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", snd_rec_ctrl_locked);
}

static ssize_t sound_control_enabled_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%d", &snd_ctrl_enabled);

	return count;
}

static ssize_t sound_control_enabled_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", snd_ctrl_enabled);
}

static struct kobj_attribute sound_reg_sel_attribute =
	__ATTR(sound_reg_sel,
		0222,
		NULL,
		sound_reg_select_store);

static struct kobj_attribute sound_reg_read_attribute =
	__ATTR(sound_reg_read,
		0444,
		sound_reg_read_show,
		NULL);

static struct kobj_attribute sound_reg_write_attribute =
	__ATTR(sound_reg_write,
		0222,
		NULL,
		sound_reg_write_store);

static struct kobj_attribute cam_mic_gain_attribute =
	__ATTR(gpl_cam_mic_gain,
		0666,
		cam_mic_gain_show,
		cam_mic_gain_store);

static struct kobj_attribute mic_gain_attribute =
	__ATTR(gpl_mic_gain,
		0666,
		mic_gain_show,
		mic_gain_store);

static struct kobj_attribute speaker_gain_attribute =
	__ATTR(gpl_speaker_gain,
		0666,
		speaker_gain_show,
		speaker_gain_store);

static struct kobj_attribute headphone_gain_attribute =
	__ATTR(gpl_headphone_gain,
		0666,
		headphone_gain_show,
		headphone_gain_store);

/*static struct kobj_attribute headphone_pa_gain_attribute =
	__ATTR(gpl_headphone_pa_gain,
		0666,
		headphone_pa_gain_show,
		headphone_pa_gain_store);*/

static struct kobj_attribute sound_control_rec_locked_attribute =
	__ATTR(gpl_sound_control_rec_locked,
		0666,
		sound_control_rec_locked_show,
		sound_control_rec_locked_store);

static struct kobj_attribute sound_control_version_attribute =
	__ATTR(gpl_sound_control_version,
		0444,
		sound_control_version_show, NULL);

/*static struct kobj_attribute sound_hw_revision_attribute =
	__ATTR(gpl_sound_control_hw_revision,
		0444,
		sound_control_hw_revision_show, NULL);*/

static struct kobj_attribute sound_control_enabled_attribute =
	__ATTR(gpl_sound_control_enabled,
		0666,
		sound_control_enabled_show,
		sound_control_enabled_store);

static struct attribute *sound_control_attrs[] =
	{
		&cam_mic_gain_attribute.attr,
		&mic_gain_attribute.attr,
		&speaker_gain_attribute.attr,
		&headphone_gain_attribute.attr,
		//&headphone_pa_gain_attribute.attr,
		&sound_control_rec_locked_attribute.attr,
		&sound_reg_sel_attribute.attr,
		&sound_reg_read_attribute.attr,
		&sound_reg_write_attribute.attr,
		//&sound_hw_revision_attribute.attr,
		&sound_control_version_attribute.attr,
		&sound_control_enabled_attribute.attr,
		NULL,
	};

static struct attribute_group sound_control_attr_group =
	{
		.attrs = sound_control_attrs,
	};

static struct kobject *sound_control_kobj;

static int sound_control_init(void)
{
	int sysfs_result;

	sound_control_kobj =
		kobject_create_and_add("sound_control_3", kernel_kobj);

	if (!sound_control_kobj) {
		pr_err("%s sound_control_kobj create failed!\n",
			__FUNCTION__);
		return -ENOMEM;
	}

	sysfs_result = sysfs_create_group(sound_control_kobj,
			&sound_control_attr_group);

	if (sysfs_result) {
		pr_info("%s sysfs create failed!\n", __FUNCTION__);
		kobject_put(sound_control_kobj);
	}

	return sysfs_result;
}

static void sound_control_exit(void)
{
	if (sound_control_kobj != NULL)
		kobject_put(sound_control_kobj);
}
module_init(sound_control_init);
module_exit(sound_control_exit);

MODULE_LICENSE("GPLv2");
MODULE_AUTHOR("Paul Reioux <reioux@gmail.com>");
MODULE_DESCRIPTION("WCD93xx Sound Engine v4.0");
