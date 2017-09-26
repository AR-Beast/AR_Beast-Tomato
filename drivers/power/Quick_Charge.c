/* Quick Charge v1.0, a Fast-Charge Driver Based on the Idea of Qualcomm's 
 * Quick-Charge Technology.
 *
 * For this Driver to Work properly, a Wall-Charger Capable of Supplying 
 * 1.5A Output is Required.
 *
 * Copyright (c) 2017, Shoaib Anwar <Shoaib0595@gmail.com>.
 *
 * Based on ThunderCharge Current Control, by Varun Chitre <varun.chitre15@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/Quick_Charge.h>

#define ENABLED			1
#define QUICK_CHARGE		"Quick_Charge"
#define CURRENT			1250

// Enable/Disable Toggle.
int QC_Toggle = ENABLED;
// Variable to Store Different Values of Current (mA).
int Dynamic_Current = CURRENT;
// Variable to Know the Status of Charging (i.e., Charger Connected or Dis-Connected).
int Charge_Status;
// Variable to Store Actual Current (mA) Drawn from AC or USB Charger.
unsigned int Actual_Current;

// Function to Read the Status (%) of Battery.
void batt_level (int Battery_Status)
{
	// Mechanism of Driver to Allocate Current (mA).
	if (Battery_Status >= 0 && Battery_Status <= 60)
	   Dynamic_Current = 1500;
	else if (Battery_Status >= 61 && Battery_Status <= 90)
		Dynamic_Current = 1250;
	else if (Battery_Status >= 91 && Battery_Status <= 100)
  	        Dynamic_Current = 1000;
}

// Function to Read the Status (Charger Connected or Dis-Connected) of Charging.
void charging (int flag)
{
	Charge_Status = flag;
}

// Function to Read the Actual Current (mA) Reported by the Charging ICs.
void actual_current (int Value)
{
	// Store Actual Current (mA) only when Battery is being Charged.
	if (Charge_Status == 1)
	   Actual_Current = Value;
        else
	    Actual_Current = 0;
}

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

static ssize_t actual_current_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u", Actual_Current);
}

static struct kobj_attribute qc_toggle_attribute =
	__ATTR(QC_Toggle,
		0666,
		qc_toggle_show,
		qc_toggle_store);

static struct kobj_attribute actual_current_attribute =
	__ATTR(Actual_Current,
		0444,
		actual_current_show, 
		NULL);

static struct attribute *charger_control_attrs[] =
	{
		&qc_toggle_attribute.attr,
		&actual_current_attribute.attr,
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
MODULE_DESCRIPTION("Quick Charge v1.0");
