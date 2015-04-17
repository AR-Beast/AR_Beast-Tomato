 #
 # Copyright © 2016, Varun Chitre "varun.chitre15" <varun.chitre15@gmail.com>
 # Copyright © 2016, Ayush Rathore "AyushR1" <ayushrathore12501@gmail.com>
 # If Anyone Use this Scrips Maintain Proper Credits
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
 # AR_Beast kernel Build script
KERNEL_DIR=$PWD
KERN_IMG=$KERNEL_DIR/arch/arm64/boot/Image
DTBTOOL=$KERNEL_DIR/tools/dtbToolCM
FINAL_KERNEL_ZIP=AR_Beast™-$(date +"%Y%m%d-%T")-lettuce.zip
ZIP_MAKER_DIR=/home/ayushr1/AR_Beast/ARB-Anykernel2
VERSION=6

BUILD_START=$(date +"%s")
blue='\033[0;34m'
cyan='\033[0;36m'
yellow='\033[0;33m'
red='\033[0;31m'
nocol='\033[0m'

export CROSS_COMPILE="/home/ayushr1/AR_Beast/GCC-UBER-Prebuilts-8/bin/aarch64-linux-android-"
export ARCH=arm64
export USE_CCACHE=1
export SUBARCH=arm64
export KBUILD_BUILD_USER="Ayush"
export KBUILD_BUILD_HOST="Beast"
STRIP="/home/ayushr1/AR_Beast/GCC-UBER-Prebuilts-8/bin/aarch64-linux-android-strip"
MODULES_DIR=$KERNEL_DIR/drivers/staging/prima/
CAL=$(($VERSION - 1))
touch .version
echo "$CAL" > .version;

compile_kernel ()
{
echo -e "$blue***********************************************"
echo "          Compiling AR_Beast™          "
echo -e "***********************************************$nocol"
rm -f $KERN_IMG
make ARBeast_lettuce_defconfig -j$(nproc --all)
make Image -j$(nproc --all)
make dtbs -j$(nproc --all)
if ! [ -a $KERN_IMG ];
then
echo -e "$red Kernel Compilation failed! Fix the errors! $nocol"
exit 1
fi
$DTBTOOL -2 -o $KERNEL_DIR/arch/arm64/boot/dt.img -s 2048 -p $KERNEL_DIR/scripts/dtc/ $KERNEL_DIR/arch/arm/boot/dts/
}

case $1 in
clean)
make ARCH=arm64 -j4 clean mrproper
;;
dt)
make ARBeast_lettuce_defconfig -j$(nproc --all)
$DTBTOOL -2 -o $KERNEL_DIR/arch/arm64/boot/dt.img -s 2048 -p $KERNEL_DIR/scripts/dtc/ $KERNEL_DIR/arch/arm/boot/dts/
;;
*)
compile_kernel
;;
esac
BUILD_END=$(date +"%s")
DIFF=$(($BUILD_END - $BUILD_START))
echo -e "$yellow Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds.$nocol"

#ZIP MAKER time!
echo "**** Verifying ZIP MAKER Directory ****"
echo "**** Removing leftovers ****"
rm -rf $ZIP_MAKER_DIR/tools/dt.img
rm -rf $ZIP_MAKER_DIR/Image

echo "**** Copying Image ****"
cp $KERNEL_DIR/arch/arm64/boot/Image $ZIP_MAKER_DIR/
mv $ZIP_MAKER_DIR/{Image,zImage}
echo "**** Copying dtb ****"
cp $KERNEL_DIR/arch/arm64/boot/dt.img $ZIP_MAKER_DIR/
echo "**** Copying tweaks ****"
cp $KERNEL_DIR/spectrum/init.spectrum.sh $ZIP_MAKER_DIR/ramdisk/
cp $KERNEL_DIR/spectrum/init.spectrum.rc $ZIP_MAKER_DIR/ramdisk/


echo "**** Time to zip up! ****"
cd $ZIP_MAKER_DIR/
git checkout lettuce
zip -r9 $FINAL_KERNEL_ZIP * -x README $FINAL_KERNEL_ZIP
rm -rf /home/beast12/ARBeast/$FINAL_KERNEL_ZIP
cp /home/ayushr1/AR_Beast/ARB-Anykernel2/$FINAL_KERNEL_ZIP /home/ayushr1/AR_Beast/out/lettuce/$FINAL_KERNEL_ZIP

echo "**** Good Bye!! ****"
rm -rf $KERNEL_DIR/arch/arm64/boot/dt.img
rm -rf $ZIP_MAKER_DIR/ramdisk/init.spectrum.sh
rm -rf $ZIP_MAKER_DIR/ramdisk/init.spectrum.rc
rm -rf $ZIP_MAKER_DIR/dt.img
rm -rf $ZIP_MAKER_DIR/zImage
rm -rf $ZIP_MAKER_DIR/$FINAL_KERNEL_ZIP

# Clearing For Commiting
echo "**** Cleaning ****"
cd $KERNEL_DIR
make clean && make mrproper
