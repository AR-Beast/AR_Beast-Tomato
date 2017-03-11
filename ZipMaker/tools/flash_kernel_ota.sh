#!/system/bin/sh
 #
 # Copyright © 2014, Varun Chitre "varun.chitre15" <varun.chitre15@gmail.com> 
 #
 # Live ramdisk patching script
 #
 # This software is licensed under the terms of the GNU General Public
 # License version 2, as published by the Free Software Foundation, and
 # may be copied, distributed, and modified under those terms.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # Please maintain this if you use this script or any part of it
 #
cd /data/local/tmp/tools/
dd if=/dev/block/bootdevice/by-name/boot of=./boot.img
./unpackbootimg -i /data/local/tmp/tools/boot.img
./mkbootimg --kernel /data/local/tmp/tools/zImage --ramdisk /data/local/tmp/tools/boot.img-ramdisk.gz --cmdline "console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 androidboot.hardware=qcom msm_rtb.filter=0x237 ehci-hcd.park=3 androidboot.bootdevice=7824900.sdhci lpm_levels.sleep_disabled=1 earlyprintk"  --base 0x80000000 --pagesize 2048 --ramdisk_offset 0x02000000 --tags_offset 0x01e00000 --dt /data/local/tmp/tools/dt.img -o /data/local/tmp/tools/newboot.img
dd if=/data/local/tmp/tools/newboot.img of=/dev/block/bootdevice/by-name/boot
mount -o rw,remount /system
rm /system/lib/modules/*.ko
cp ../system/lib/modules/* /system/lib/modules
rm /system/vendor/lib/hw/power.msm8916.so
rm /system/vendor/lib64/hw/power.msm8916.so
rm -rf /data/local/tmp/*
exit 22
