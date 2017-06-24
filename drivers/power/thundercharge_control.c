/*
 * Copyright © 2015, Varun Chitre "varun.chitre15" <varun.chitre15@gmail.com>
 *
 * Copyright © 2017, Ayush Rathore 
 * 
 * Charger Control driver for yl_bq24157_charger and yl_fan5405_charger
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include "thundercharge_control.h"

#define ENABLED             1
#define USB_CURRENT         1000
#define MAX_VBUS_CURRENT    1500
#define THUNDERCHARGE       "thundercharge"

int mswitch = ENABLED;
int custom_usb_current = USB_CURRENT;

#define DRIVER_VERSION  3
#define DRIVER_SUBVER   0

static ssize_t mswitch_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", mswitch);
}

static ssize_t mswitch_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val;
	sscanf(buf, "%d", &val);
	switch(val)
	{
	case 0:
	case 1:
		mswitch = val;
	break;
	default:
		pr_info("%s: invalid value specified", THUNDERCHARGE);
    break;
}

return count;
}

static ssize_t chgr_ver_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "Charger Control %u.%u", DRIVER_VERSION, DRIVER_SUBVER);
}

static struct kobj_attribute mswitch_attribute =
	__ATTR(enabled,
		0666,
		mswitch_show,
		mswitch_store);
		
static struct kobj_attribute chgr_ctrl_ver_attribute =
	__ATTR(version,
		0444,
		chgr_ver_show, NULL);

static struct attribute *charger_control_attrs[] =
	{
		&mswitch_attribute.attr,
		&chgr_ctrl_ver_attribute.attr,
		NULL,
	};
	
static struct attribute_group chgr_control_attr_group =
	{
		.attrs = charger_control_attrs,
	};

static struct kobject *charger_control_kobj;

static int charger_control_probe(void)
{
	int sysfs_result;
	printk(KERN_DEBUG "[%s]\n",__func__);

	charger_control_kobj = kobject_create_and_add("thundercharge_control", kernel_kobj);

	if (!charger_control_kobj) {
		pr_err("%s Interface create failed!\n",
			__FUNCTION__);
		return -ENOMEM;
        }

	sysfs_result = sysfs_create_group(charger_control_kobj,
			&chgr_control_attr_group);

	if (sysfs_result) {
		pr_info("%s sysfs create failed!\n", __FUNCTION__);
		kobject_put(charger_control_kobj);
	}
	return sysfs_result;
}

static void charger_control_remove(void)
{
	if (charger_control_kobj != NULL)
		kobject_put(charger_control_kobj);
}

module_init(charger_control_probe);
module_exit(charger_control_remove);
MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Varun Chitre <varun.chitre15@gmail.com>");
MODULE_DESCRIPTION("BQ24157/FAN5405 Charger control driver");
