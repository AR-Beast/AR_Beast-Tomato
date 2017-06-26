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

#define ENABLED             1
#define QUICK_CHARGE        "Quick_Charge"
#define CURRENT     	    1250
#define USBCURRENT          1000

// Enable/Disable Toggle.
int QC_Toggle = ENABLED;

// Variable to Store Different Values of Current (mA).
int Dynamic_Current = CURRENT;

// Variable to Store a Copy of Dynamic Current (mA).
int Mirror_Current;

// Variable to Store Value of USB Current (mA).
int USB_Current = USBCURRENT;

// Variable to Know the Status of Charging (i.e., Charger Connected or Dis-Connected).
int Charge_Status;

// Variable to Know the Status of Charging (i.e., Charger Connected or Dis-Connected).
int custom_current = 1500;

// Function to Read the Status (%) of Battery.
void batt_level (int Battery_Status)
{
	// Mechanism of Driver to Allocate Current (mA).
	if (Battery_Status >= 0 && Battery_Status <= 75)
	{
	   Dynamic_Current = custom_current;
	   Mirror_Current = custom_current;
	}
	else if (Battery_Status >= 76 && Battery_Status <= 93)
	{
		Dynamic_Current = (custom_current - 120);
		Mirror_Current = (custom_current - 120);
	}
	else if (Battery_Status >= 94 && Battery_Status <= 100)
	{
  	    Dynamic_Current = (custom_current - 250);
		Mirror_Current = (custom_current - 250);
	}
}

// Function to Read the Status (Charger Connected or Dis-Connected) of Charging.
void charging (int flag)
{
	Charge_Status = flag;

	if (Charge_Status == 1)
	   // Read the Current (mA) Value from Copy-Variable.
	   Dynamic_Current = Mirror_Current;
	else
	   // If the Battery is Dis-Charging, show 0 mA.
	   Dynamic_Current = 0;
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
	       // If the Driver is Disabled, show 0 mA.
	       Dynamic_Current = val;
	break;
	case 1:
	       QC_Toggle = val;
	       // If the Driver is Enabled and the Battery is Charging, then only, read the Current (mA) Value from Copy-Variable.
	       if (Charge_Status == 1)
	          Dynamic_Current = Mirror_Current;
	break;
	default:
		pr_info("%s: Invalid Value", QUICK_CHARGE);
    	break;
	}
return count;
}

static ssize_t dynamic_current_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", Dynamic_Current);
}

static ssize_t custom_current_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", custom_current);
}

static ssize_t custom_current_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int usercurrent;
	sscanf(buf, "%d", &usercurrent);
	if(QC_Toggle == 1 && usercurrent <= 1500 && usercurrent  >= 1250)
		custom_current = usercurrent;
	else
		pr_info("%s: disabled or limit reached, ignoring\n", QUICK_CHARGE);
	return count;
}

static ssize_t usb_current_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", USB_Current);
}

static ssize_t usb_current_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int userusbcurrent;
	sscanf(buf, "%d", &userusbcurrent);
	if(QC_Toggle == 1 && userusbcurrent <= 1000 && userusbcurrent  >= 500)
		USB_Current = userusbcurrent;
	else
		pr_info("%s: disabled or limit reached, ignoring\n", QUICK_CHARGE);
	return count;
}

static struct kobj_attribute qc_toggle_attribute =
	__ATTR(QC_Toggle,
		0666,
		qc_toggle_show,
		qc_toggle_store);

static struct kobj_attribute dynamic_current_attribute =
	__ATTR(Dynamic_Current,
		0444,
		dynamic_current_show, 
		NULL);

static struct kobj_attribute custom_current_attribute =
	__ATTR(custom_current,
		0666,
		custom_current_show,
		custom_current_store);

static struct kobj_attribute usb_current_attribute =
	__ATTR(USB_Current,
		0666,
		usb_current_show,
		usb_current_store);

static struct attribute *charger_control_attrs[] =
	{
		&qc_toggle_attribute.attr,
		&dynamic_current_attribute.attr,
		&custom_current_attribute.attr,
		&usb_current_attribute,
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
