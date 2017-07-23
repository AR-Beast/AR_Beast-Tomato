/* Quick Charge+, a Current-Booster Driver for Qualcomm's Quick-Charge Technology.
 *
 * Copyright (c) 2017, Shoaib Anwar <Shoaib0595@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Fast Charge
 * Author: Chad Froebel <chadfroebel@gmail.com>
 * Port to Nexus 5 : flar2 <asegaert@gmail.com>
 * Port to Osprey : engstk <eng.stk@sapo.pt>
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
 *
 *
 * Possible values for "force_fast_charge" are :
 *
 *   0 - Disabled (default)
 *   1 - Force faster charge
*/

#include <linux/module.h>
#include <linux/Quick_Charge.h>
#include <linux/kobject.h>
#include <linux/fastchg.h>
#include <linux/string.h>

#define ENABLED             1
#define QUICK_CHARGE        "Quick_Charge"

// Enable/Disable Toggle.
int QC_Toggle = ENABLED;

int force_fast_charge = ENABLED;

//Force Fast Charge
static int __init get_fastcharge_opt(char *ffc)
{
	if (strcmp(ffc, "0") == 0) {
		force_fast_charge = 0;
	} else if (strcmp(ffc, "1") == 0) {
		force_fast_charge = 1;
	} else {
		force_fast_charge = 0;
	}
	return 1;
}

__setup("ffc=", get_fastcharge_opt);

static ssize_t qc_toggle_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", QC_Toggle);
}

static ssize_t qc_toggle_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int val;
	sscanf (buf, "%d", &val);
	switch (val)
	{
	case 0:
	       QC_Toggle = val;
	break;
	case 1:
	       QC_Toggle = val;
	break;
	default:
		pr_info("%s: Invalid Value", QUICK_CHARGE);
    	break;
	}
return count;
}

static struct kobj_attribute qc_toggle_attribute =
	__ATTR(QC_Toggle,
		0666,
		qc_toggle_show,
		qc_toggle_store);

static struct attribute *charger_control_attrs[] =
	{
		&qc_toggle_attribute.attr,
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

	charger_control_kobj = kobject_create_and_add("Quick_Charge", kernel_kobj);

	if (!charger_control_kobj) 
	{
	   pr_err("%s Interface create failed!\n", __FUNCTION__);
	   return -ENOMEM;
        }

	sysfs_result = sysfs_create_group(charger_control_kobj, &chgr_control_attr_group);

	if (sysfs_result) 
	{
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
MODULE_LICENSE("GPL and Additional Rights");
MODULE_AUTHOR("Shoaib Anwar <Shoaib0595@gmail.com>");
MODULE_DESCRIPTION("Quick Charge+");
