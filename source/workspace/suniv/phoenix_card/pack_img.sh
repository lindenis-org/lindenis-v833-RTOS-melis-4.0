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
# Last Modified:  2018-11-24 14:04:01
#
#=====================================================================================
#!/bin/sh

function update()
{
	echo "-----------------------------------"
	echo "             update                "
	echo "-----------------------------------"

	cp -f ../eGon/storage_media/sdcard/boot0.bin boot0.bin
	cp -f ../eGon/storage_media/sdcard/boot1.bin boot1.bin
	
	if [ -e ./rootfs/ ] ; then
		rm ./rootfs -rf
	fi
	if [ -e ./ramfs/ ] ; then
		rm ./ramfs -rf
	fi

	cp ../rootfs rootfs/ -rf
	cp ../ramfs ramfs/ -rf

	cp rootfs/drv/sdmmc2.drv ramfs/drv/sdmmc2.drv
	cp rootfs/apps/init_card.axf rootfs/apps/init.axf
}

function boot_checksum()
{
	echo "-----------------------------------"
	echo "             boot checksum         "
	echo "-----------------------------------"
	$PACK_PATH/checksum 0 ./../eGon/storage_media/sdcard/booter0.bin ./../eGon/storage_media/sdcard/boot0.bin
	$PACK_PATH/checksum 1 ./../eGon/storage_media/sdcard/booter1.bin ./../eGon/storage_media/sdcard/boot1.bin
}

function script_config()
{
	echo "-----------------------------------"
	echo "             script                "
	echo "-----------------------------------"
	$PACK_PATH/script ./../eFex/config/sys_config.fex
	$PACK_PATH/script ./../eFex/config/sys_partition.fex
}

function update_boot_card()
{
	#update_boot1 have some error at script_data
	echo "-----------------------------------"
	echo "       update boot0\boot1          "
	echo "-----------------------------------"

	echo "update $BOARD_NAME"
	$PACK_PATH/update_boot0_$PLATFORM_NAME boot0.bin  ../eFex/config/sys_config.bin  SDMMC_CARD
	$PACK_PATH/update_boot1 boot1.bin  ../eFex/config/sys_config.bin  SDMMC_CARD
}


function do_mbr()
{
	echo "-----------------------------------"
	echo "              生成MBR文件          "
	echo "-----------------------------------"
	$PACK_PATH/script part_config.fex
	$PACK_PATH/BurnMBR part_config.bin  mbr.fex
	#$PACK_PATH/update_mbr part_config.bin mbr.fex
}

function do_fes1_uboot_update()
{
	cp -f ../eFex/usb/fes1.bin fes1.fex   
	cp -f ../eFex/usb/u-boot.bin u-boot.fex

	#update_uboot have some error
	echo "-----------------------------------"
	echo "           update fes1 uboot       "
	echo "-----------------------------------"
	$PACK_PATH/update_fes1  fes1.fex  ../eFex/config/sys_config.bin  
	$PACK_PATH/update_uboot u-boot.fex  ../eFex/config/sys_config.bin
}

function do_fs()
{
	echo "-----------------------------------"
	echo "           make filesystem image   "
	echo "-----------------------------------"
	
	echo " make ramfs filesystem "
	$PACK_PATH/fsbuild ./ramfs.ini
	
	echo " make rootfs filesystem "
	$PACK_PATH/fsbuild ./rootfs.ini ../eFex/split_c43a2a92-a8ed-4f92-abdb-ca0d5d29666d.bin 
}

function do_fix_file()
{
	$PACK_PATH/fix_file boot0.bin  21
	$PACK_PATH/fix_file boot1.bin  71
	mv boot0.bin boot0.fex
	mv boot1.bin boot1.fex
}

function do_checksum()
{
	cat rootfs.fex > melis100.fex

	echo "-----------------------------------"
	echo "               check sum           "
	echo "-----------------------------------"
	$PACK_PATH/FileAddSum melis100.fex verify.fex
}

function do_package()
{
	echo "-----------------------------------"
	echo "              package              "
	echo "-----------------------------------"
	#$PACK_PATH/compile -o image.bin image.cfg
	#$PACK_PATH/dragon  image.cfg ./../eFex/config/sys_partition.fex
	$PACK_PATH/dragon  image.cfg
}

function do_finish()
{
	rm -fr  boot0.fex
	rm -fr  boot1.fex
	rm -fr  melis100.fex
	rm -fr  u-boot.fex
	rm -fr  fes1.fex
	#rm -fr  mbr.bin
	#rm -fr  sunxi_mbr.fex
	rm -fr  rootfs.fex
	rm -fr  ../eFex/config/sys_partition.bin
	rm -fr  ../eFex/config/sys_config.bin
	#rm -fr  dlinfo.fex
	rm -rf verify.fex
	rm -rf part_config.bin

	rm -rf ../eFex/usb
	
	if [ -e ./rootfs/ ] ; then
		rm ./rootfs -rf
	fi
	if [ -e ./ramfs/ ] ; then
		rm ./ramfs -rf
	fi
	echo "*****make image end*****"
}

function do_init_card()
{
	echo "*****make image begin*****"
	ImagePath="PHOENIXv100.img"
	if [ -f "$ImagePath" ]; then
	 	rm -fr "$ImagePath"
	fi

	cp ../eFex/usb_$PLATFORM_NAME ../eFex/usb -rf
}


PACK_PATH=$(cd "$(dirname "$0")"; pwd)/../beetles/packtool

if [ -z $MELIS_BASE ];then  
	echo "MELIS_BASE Environment Not Exists!"  
	exit -1;
fi 

. $MELIS_BASE/.config
. ../beetles/select.sh

do_select_platform

do_init_card

boot_checksum
update

if [ $CONFIG_MINIGUI_SAMPLE ]; then
	echo "copy the minigui resource files to res"
	cp -r minigui_res/* rootfs/res/
	#cp -f ./../rootfs/drv/sample.drv              ./rootfs/drv/sample.drv
	rm ./rootfs/mod/orange.mod
	rm ./rootfs/apps/app_root.axf
	rm ./rootfs/apps/init.axf
	rm ./rootfs/apps/theme.bin
else
	mv rootfs/res/fonts/ temp_fonts/
	mv rootfs/res/sounds/ temp_sounds/
	rm -rf rootfs/res/*
	mv temp_fonts/ rootfs/res/fonts/
	mv temp_sounds/ rootfs/res/sounds/

	rm ./rootfs/drv/sample.drv
fi

script_config

update_boot_card

do_mbr
do_fes1_uboot_update
do_fs
do_fix_file
do_checksum
do_package
do_finish

if [ $CONFIG_MINIGUI_SAMPLE ]; then
	echo "delete the minigui resource files"	
	mv rootfs/res/fonts/ temp_fonts/
	mv rootfs/res/sounds/ temp_sounds/
	rm -rf rootfs/res/*
	mv temp_fonts/ rootfs/res/fonts/
	mv temp_sounds/ rootfs/res/sounds/
	rm ./rootfs/drv/sample.drv
fi
