#=====================================================================================
#
#      Filename:  pack.sh
#
#   Description:  pack script to generate firmware.
#
#       Version:  2.0 
#        Create:  2017-12-22 16:57:46
#      Revision:  none
#      Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
#
#        Author:  zhijinzeng@allwinnertech.com
#  Organization:  BU1-PSW
# Last Modified:  2019-10-23 16:19:36
#
#=====================================================================================
#!/bin/sh

function update()
{
	echo "-----------------------------------"
	echo "             update                "
	echo "-----------------------------------"

	cp -f $PACK_WORKDIR/../eGon/storage_media/spinor/boot0.bin $PACK_WORKDIR/boot0.bin
	cp -f $PACK_WORKDIR/../eGon/storage_media/spinor/boot1.bin $PACK_WORKDIR/boot1.bin
	
	if [ -e $PACK_WORKDIR/rootfs/ ] ; then
		rm $PACK_WORKDIR/rootfs -rf
	fi
	if [ -e $PACK_WORKDIR/ramfs/ ] ; then
		rm $PACK_WORKDIR/ramfs -rf
	fi

	cp $PACK_WORKDIR/../rootfs $PACK_WORKDIR/rootfs/ -rf
	cp $PACK_WORKDIR/../ramfs $PACK_WORKDIR/ramfs -rf
}

function boot_checksum()
{
	echo "-----------------------------------"
	echo "             boot checksum         "
	echo "-----------------------------------"
	$PACK_PATH/checksum 0 $PACK_WORKDIR/../eGon/storage_media/spinor/booter0.bin $PACK_WORKDIR/../eGon/storage_media/spinor/boot0.bin
	$PACK_PATH/checksum 1 $PACK_WORKDIR/../eGon/storage_media/spinor/booter1.bin $PACK_WORKDIR/../eGon/storage_media/spinor/boot1.bin
}

function script_config()
{
	echo "-----------------------------------"
	echo "             script                "
	echo "-----------------------------------"

	$PACK_PATH/script ../eFex/config/sys_config.fex
	$PACK_PATH/script ../eFex/config/sys_partition.fex
}

function update_boot()
{
	#update_boot1 have some error at script_data
	echo "-----------------------------------"
	echo "       update boot0\boot1          "
	echo "-----------------------------------"

	echo "update $BOARD_NAME"
	$PACK_PATH/update_boot0_$PLATFORM_NAME $PACK_WORKDIR/boot0.bin  $PACK_WORKDIR/../eFex/config/sys_config.bin  SPINOR_FLASH
	$PACK_PATH/update_boot1 $PACK_WORKDIR/boot1.bin  $PACK_WORKDIR/../eFex/config/sys_config.bin  SPINOR_FLASH
}

function do_mbr()
{
	echo "-----------------------------------"
	echo "              生成MBR文件          "
	echo "-----------------------------------"
	$PACK_PATH/BurnMBR $PACK_WORKDIR/../eFex/config/sys_config.bin  $PACK_WORKDIR/mbr.bin
	$PACK_PATH/update_mbr $PACK_WORKDIR/../eFex/config/sys_partition.bin  4
}

function do_fes1_uboot_update()
{
	cp -f $PACK_WORKDIR/../eFex/usb/fes1.bin $PACK_WORKDIR/fes1.fex   
	cp -f $PACK_WORKDIR/../eFex/usb/u-boot.bin $PACK_WORKDIR/u-boot.fex

	#update_uboot have some error
	echo "-----------------------------------"
	echo "           update fes1 uboot       "
	echo "-----------------------------------"
	$PACK_PATH/update_fes1  $PACK_WORKDIR/fes1.fex  $PACK_WORKDIR/../eFex/config/sys_config.bin  
	$PACK_PATH/update_uboot $PACK_WORKDIR/u-boot.fex  $PACK_WORKDIR/../eFex/config/sys_config.bin
}

function do_fs()
{
	$PACK_PATH/minfs make ramfs $PACK_WORKDIR/./rootfs/ramdisk.iso $PACK_WORKDIR/ramfs.ini
	$PACK_PATH/minfs make rootfs $PACK_WORKDIR/rootfs.iso $PACK_WORKDIR/rootfs.ini
	$PACK_PATH/fsbuild $PACK_WORKDIR/./udisk.ini
}

function do_fix_file()
{
	$PACK_PATH/fix_file $PACK_WORKDIR/boot0.bin  24
	$PACK_PATH/fix_file $PACK_WORKDIR/boot1.bin  120
}

function do_checksum()
{
	cat $PACK_WORKDIR/boot0.bin $PACK_WORKDIR/boot1.bin $PACK_WORKDIR/mbr.bin $PACK_WORKDIR/rootfs.iso $PACK_WORKDIR/udisk.iso > melis100.fex

	echo "-----------------------------------"
	echo "               check sum           "
	echo "-----------------------------------"
	$PACK_PATH/FileAddSum $PACK_WORKDIR/melis100.fex $PACK_WORKDIR/verify.fex
}

function do_package()
{
	echo "-----------------------------------"
	echo "              package              "
	echo "-----------------------------------"
	$PACK_PATH/dragon  $PACK_WORKDIR/image.cfg $PACK_WORKDIR/./../eFex/config/sys_partition.fex
}

function do_finish()
{
	#rm -fr $PACK_WORKDIR/boot0.bin
	#rm -fr $PACK_WORKDIR/boot1.bin
	#rm -fr $PACK_WORKDIR/melis100.fex
	rm -fr  $PACK_WORKDIR/u-boot.fex
	rm -fr  $PACK_WORKDIR/fes1.fex
	rm -fr  $PACK_WORKDIR/mbr.bin
	rm -fr  $PACK_WORKDIR/sunxi_mbr.fex
	#rm -fr  $PACK_WORKDIR/rootfs.iso
	rm -fr  $PACK_WORKDIR/udisk.iso
	rm -fr  $PACK_WORKDIR/../eFex/config/sys_partition.bin
	rm -fr  $PACK_WORKDIR/../eFex/config/sys_config.bin
	rm -fr  $PACK_WORKDIR/dlinfo.fex

	#rm -rf $PACK_WORKDIR/../eFex/sys_config_per1_v1.fex
	rm -rf  $PACK_WORKDIR/../eFex/usb
	
	if [ -e $PACK_WORKDIR/./rootfs/ ] ; then
		rm $PACK_WORKDIR/./rootfs -rf
	fi
	if [ -e $PACK_WORKDIR/./ramfs/ ] ; then
		rm $PACK_WORKDIR/./ramfs -rf
	fi

	IMG_NAME="ePDKv100.img"
    	echo -e '\033[0;31;1m'
    	echo $MELIS_BASE/workspace/suniv/beetles/${IMG_NAME}
    	echo -e '\033[0m'

	echo "*****make image end*****"
}

function do_init()
{
	echo "*****make image begin*****"
	Images=$PACK_WORKDIR/ePDKv100.img
	if [ -f $Images ]; then
	 	rm -fr $Images
	fi

	cp $PACK_WORKDIR/../eFex/usb_$PLATFORM_NAME $PACK_WORKDIR/../eFex/usb -rf
}

PACK_WORKDIR=$MELIS_BASE/workspace/suniv/beetles
PACK_PATH=$PACK_WORKDIR/packtool

if [ -z $MELIS_BASE ];then  
	echo "MELIS_BASE Environment Not Exists!"  
	exit -1;
fi 

. $MELIS_BASE/.config
. $PACK_WORKDIR/select.sh

do_select_platform
do_init
boot_checksum
update

mv $PACK_WORKDIR/rootfs/res/fonts/ $PACK_WORKDIR/temp_fonts/
mv $PACK_WORKDIR/rootfs/res/sounds/ $PACK_WORKDIR/temp_sounds/
rm -rf $PACK_WORKDIR/rootfs/res/*
mv $PACK_WORKDIR/temp_fonts/ $PACK_WORKDIR/rootfs/res/fonts/
mv $PACK_WORKDIR/temp_sounds/ $PACK_WORKDIR/rootfs/res/sounds/

script_config
update_boot
do_mbr
do_fes1_uboot_update
do_fs
do_fix_file
do_checksum
do_package
do_finish
