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
# Last Modified:  2020-08-18 11:57:16
#
#=====================================================================================
#!/bin/sh

function do_init_prepare()
{
	echo "-----------------------------------"
	echo "         do init prepare           "
	echo "-----------------------------------"

	if [ x$PLATFORM_NAME == "xsun8iw19" ]; then
		local USB_BURN_FILE_PATH=$MELIS_BASE/workspace/suniv/eFex/usb_$PLATFORM_NAME
		local CAED_BURN_FILE_PATH=$MELIS_BASE/workspace/suniv/eFex/card_$PLATFORM_NAME

		echo "---------copy $PLATFORM_NAME usb burn file------------"
		cp $USB_BURN_FILE_PATH/aultls32.fex $PACK_WORKDIR
		cp $USB_BURN_FILE_PATH/aultools.fex $PACK_WORKDIR
		cp $USB_BURN_FILE_PATH/split_xxxx.fex $PACK_WORKDIR
		cp $USB_BURN_FILE_PATH/usbtool.fex $PACK_WORKDIR
		cp $USB_BURN_FILE_PATH/usbtool_crash.fex $PACK_WORKDIR

		echo "---------copy $PLATFORM_NAME card burn file------------"
		cp $CAED_BURN_FILE_PATH/boot0_sdcard.bin $PACK_WORKDIR/boot0_sdcard.fex
		cp $CAED_BURN_FILE_PATH/cardscript.fex $PACK_WORKDIR
		cp $CAED_BURN_FILE_PATH/cardtool.fex $PACK_WORKDIR

		echo "---------copy $TARGET_BOARD config file------------"
		cp $CONFIG_PATH/* $PACK_WORKDIR -rf
		if [ $? -ne 0 ]; then
			echo "copy config file fail"
		fi
	fi
	[ -f env.cfg ] && {
		env_size=4096
		$PACK_PATH/mkenvimage -r -p 0x00 -s ${env_size} -o env.fex env.cfg
	}
}

function update()
{
	echo "-----------------------------------"
	echo "             update                "
	echo "-----------------------------------"

	cp -f $PACK_WORKDIR/../eGon/storage_media/spinor/boot0.bin $PACK_WORKDIR/boot0.fex
	cp -f $PACK_WORKDIR/../eGon/storage_media/spinor/booter1.bin $PACK_WORKDIR/boot1.bin
	
	if [ -e $PACK_WORKDIR/rootfs/ ] ; then
		rm $PACK_WORKDIR/rootfs -rf
	fi
	if [ -e $PACK_WORKDIR/ramfs/ ] ; then
		rm $PACK_WORKDIR/ramfs -rf
	fi

	cp $PACK_WORKDIR/../rootfs $PACK_WORKDIR/rootfs/ -rf
	cp $MELIS_BASE/projects/defconfig/$TARGET_BOARD/rootfs/ $PACK_WORKDIR/ -rf
	cp $PACK_WORKDIR/../ramfs $PACK_WORKDIR/ramfs -rf
}

function script_config()
{
	echo "-----------------------------------"
	echo "             script                "
	echo "-----------------------------------"

	busybox unix2dos sys_config.fex
	$PACK_PATH/script $CONFIG_PATH/sys_config.fex
	busybox unix2dos $CONFIG_PATH/sys_partition.fex
	$PACK_PATH/script $CONFIG_PATH/sys_partition.fex

	cp $CONFIG_PATH/sys_config.fex $PACK_WORKDIR/
	cp $CONFIG_PATH/sys_partition.fex $PACK_WORKDIR/
}

function update_boot()
{
	#update_boot1 have some error at script_data
	echo "-----------------------------------"
	echo "       update boot0\boot1          "
	echo "-----------------------------------"

	echo "update $BOARD_NAME"
	$PACK_PATH/update_boot0_$PLATFORM_NAME $PACK_WORKDIR/boot0.fex  $CONFIG_PATH/sys_config.bin  SPINOR_FLASH
	$PACK_PATH/update_boot0_$PLATFORM_NAME $PACK_WORKDIR/boot0_sdcard.fex  $CONFIG_PATH/sys_config.bin  SDMMC_CARD
	#$PACK_PATH/update_boot1 $PACK_WORKDIR/boot1.bin  $CONFIG_PATH/sys_config.bin  SPINOR_FLASH

	[ -f epos.img ] && {
		gzip -c epos.img > epos-gz.img
	}

	[ -f epos.img ] && {
		$PACK_PATH/lz4 epos.img epos-lz4.img
	}

	if [ -f boot_package_nor.cfg ]; then
		echo "pack boot nor package"
		busybox unix2dos boot_package_nor.cfg
		$PACK_PATH/dragonsecboot -pack boot_package_nor.cfg
		if [ $? -ne 0 ]
		then
			pack_error "dragon pack nor run error"
			exit 1
		fi
		mv boot_package.fex boot_package_nor.fex
	fi
	if [ -f boot_package_nor.fex -a x$PLATFORM_NAME = "xsun8iw19" ]; then
		mv boot_package_nor.fex melis_pkg_nor.fex
		# keep small boot_package_nor.fex to make uboot happy, won't write to flash
		dd if=melis_pkg_nor.fex of=boot_package_nor.fex bs=1k count=32
	fi
}

function do_mbr()
{
	echo "-----------------------------------"
	echo "              生成MBR文件          "
	echo "-----------------------------------"
	$PACK_PATH/BurnMBR $CONFIG_PATH/sys_config.bin  $PACK_WORKDIR/mbr.bin
	$PACK_PATH/update_mbr $CONFIG_PATH/sys_partition.bin 1 $PACK_WORKDIR/sunxi_mbr_nor.fex
}

function do_fes1_uboot_update()
{
	cp -f $PACK_WORKDIR/../eFex/usb/fes1.bin $PACK_WORKDIR/fes1.fex   
	cp -f $PACK_WORKDIR/../eFex/usb/u-boot.bin $PACK_WORKDIR/u-boot.fex

	#update_uboot have some error
	echo "-----------------------------------"
	echo "           update fes1 uboot       "
	echo "-----------------------------------"
	$PACK_PATH/update_fes1  $PACK_WORKDIR/fes1.fex  $CONFIG_PATH/sys_config.bin
	$PACK_PATH/update_uboot  -no_merge $PACK_WORKDIR/u-boot.fex  $CONFIG_PATH/sys_config.bin

	if [ -f boot_package.cfg ]; then
		echo "pack boot package"
		busybox unix2dos boot_package.cfg
		$PACK_PATH/dragonsecboot -pack boot_package.cfg
		if [ $? -ne 0 ]; then
			echo "dragon pack run error"
			exit 1
		fi
	fi
}

function do_fs()
{
	#$PACK_PATH/minfs make ramfs $PACK_WORKDIR/./rootfs/ramdisk.iso $PACK_WORKDIR/ramfs.ini

	if [ $CONFIG_ROOTFS_MINFS  ]; then
		$PACK_PATH/minfs make rootfs $PACK_WORKDIR/rootfs.fex $PACK_WORKDIR/rootfs.ini
	elif [ $CONFIG_ROOTFS_LITTLEFS  ]; then
		#$PACK_PATH/mklfs -c rootfs -b 4096 -r 256 -p 256 -s 0x700000 -i rootfs.fex
		$PACK_PATH/mklfs -c rootfs -b 4096 -r 256 -p 256 -s 0x400000 -i rootfs.fex
	fi

	#$PACK_PATH/fsbuild $PACK_WORKDIR/./udisk.ini
}

function do_package()
{
	echo "-----------------------------------"
	echo "              package              "
	echo "-----------------------------------"
	$PACK_PATH/dragon  $PACK_WORKDIR/image.cfg $CONFIG_PATH/sys_partition.fex
	if [ $? -ne 0 ]; then
		echo -e '\033[0;31;1m'
		echo "=== ERROR : dragon pack is unsuccessful, please check !!! ==="
		echo -e '\033[0m'
		exit 1
	fi
}

function do_create_rtos_full_img()
{
	echo "-----------------------------------"
	echo "       do create full img          "
	echo "-----------------------------------"
	#rtos use uboot-2018, so not use mbr, but gpt
	gpt_file=sunxi_gpt_nor.fex
	mbr_source_file=sunxi_mbr_nor.fex
	#rtos logic start is 112K
	LOGIC_START=48
	# UDISK_SIZE size is KB
	UDISK_SIZE=16
	# full_img size is M
	FULL_IMG_SIZE=16
	full_rtos_img_name=rtos_${FULL_IMG_SIZE}Mnor.fex

	#Delete old files and make sure the files are newly generated
	rm -fr ${gpt_file}
	rm -fr ${full_rtos_img_name}

	echo ----------------mbr convert to gpt start---------------------
	$PACK_PATH/mbr_convert_to_gpt --source ${mbr_source_file} \
								  --out ${gpt_file} \
								  --input_logic_offset $((${LOGIC_START} * 1024 / 512 )) \
								  --input_flash_size ${FULL_IMG_SIZE}
	echo ----------------mbr convert to gpt end---------------------
	if [ ! -f ${gpt_file} ]; then
		echo -e '\033[0;31;1m'
		echo "ERROR : mbr_convert_to_gpt failed"
		echo -e '\033[0m'
		exit 1
	fi

	#rtos have not uboot, rtos use gpt
	$PACK_PATH/merge_full_rtos_img --out ${full_rtos_img_name} \
								   --boot0 boot0.fex \
								   --mbr ${gpt_file} \
								   --logic_start ${LOGIC_START} \
								   --partition $CONFIG_PATH/sys_partition.bin \
								   --UDISK_partition_size ${UDISK_SIZE} \
								   --total_image_size ${FULL_IMG_SIZE}
	if [ -e ${full_rtos_img_name} ]; then
		echo -e '\033[0;31;1m'
		echo $MELIS_BASE/workspace/suniv/beetles/${full_rtos_img_name}
		echo -e '\033[0m'
	else
		echo -e '\033[0;31;1m'
		echo "ERROR : merge_full_rtos_img failed"
		echo -e '\033[0m'
		exit 1
	fi
}

function do_finish()
{
	#rm -fr $PACK_WORKDIR/boot0.bin
	#rm -fr $PACK_WORKDIR/boot1.bin
	rm -fr  $PACK_WORKDIR/u-boot.fex
	rm -fr  $PACK_WORKDIR/fes1.fex
	rm -fr  $PACK_WORKDIR/mbr.bin
	rm -fr  $PACK_WORKDIR/sunxi_mbr.fex
	rm -fr  $PACK_WORKDIR/sunxi_mbr_nor.fex
	rm -fr  $PACK_WORKDIR/sunxi_gpt_nor.fex
	rm -fr  $PACK_WORKDIR/boot_package_nor.fex
	rm -fr  $PACK_WORKDIR/boot_package.fex
	#rm -fr  $PACK_WORKDIR/melis_pkg_nor.fex
	rm -fr  $PACK_WORKDIR/Vmelis_pkg_nor.fex
	#rm -fr  $PACK_WORKDIR/rootfs.fex
	rm -fr  $PACK_WORKDIR/boot0.fex
	rm -fr  $PACK_WORKDIR/boot0_sdcard.fex
	rm -fr  $PACK_WORKDIR/boot1.fex
	rm -fr  $PACK_WORKDIR/Vrootfs.fex
	rm -fr  $PACK_WORKDIR/udisk.iso
	rm -fr  $CONFIG_PATH/sys_partition.bin
	rm -fr  $CONFIG_PATH/sys_config.bin
	rm -fr  $PACK_WORKDIR/sys_config.fex
	rm -fr  $PACK_WORKDIR/sys_partition.fex
	rm -fr  $PACK_WORKDIR/dlinfo.fex
	rm -fr  $PACK_WORKDIR/sunxi_gpt.fex
	rm -fr  $PACK_WORKDIR/Venv.fex
	rm -fr  $PACK_WORKDIR/env.fex
	rm -fr  $PACK_WORKDIR/epos-gz.img
	rm -fr  $PACK_WORKDIR/epos-lz4.img

	#==rm pack Configuration file==
	rm -fr  $PACK_WORKDIR/arisc.fex
	rm -fr  $PACK_WORKDIR/aultls32.fex
	rm -fr  $PACK_WORKDIR/aultools.fex
	rm -fr  $PACK_WORKDIR/boot_package_nor.cfg
	rm -fr  $PACK_WORKDIR/boot_package.cfg
	rm -fr  $PACK_WORKDIR/cardscript.fex
	rm -fr  $PACK_WORKDIR/cardtool.fex
	rm -fr  $PACK_WORKDIR/config.fex
	rm -fr  $PACK_WORKDIR/image.cfg
	rm -fr  $PACK_WORKDIR/split_xxxx.fex
	rm -fr  $PACK_WORKDIR/sunxi.fex
	rm -fr  $PACK_WORKDIR/env.cfg

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

	if [ x$TARGET_BOARD == "xh031qp-xy-doorbell" ]; then
	echo "copy u-boot.bin of $TARGET_BOARD to eFex/usb"
	cp $CONFIG_PATH/u-boot.bin $PACK_WORKDIR/../eFex/usb -rf
	fi
}

PACK_WORKDIR=$MELIS_BASE/workspace/suniv/beetles
PACK_PATH=$PACK_WORKDIR/packtool
CONFIG_PATH=$MELIS_BASE/projects/defconfig/$TARGET_BOARD/aw_$TARGET_BOARD/

if [ -z $MELIS_BASE ];then  
	echo "MELIS_BASE Environment Not Exists!"  
	exit -1;
fi 

. $MELIS_BASE/.config
. $PACK_WORKDIR/select.sh

do_select_platform
do_init_prepare
do_init
update

mv $PACK_WORKDIR/rootfs/res/fonts/ $PACK_WORKDIR/temp_fonts/
mv $PACK_WORKDIR/rootfs/res/sounds/ $PACK_WORKDIR/temp_sounds/
rm -rf $PACK_WORKDIR/rootfs/res/*
mv $PACK_WORKDIR/temp_fonts/ $PACK_WORKDIR/rootfs/res/fonts/
mv $PACK_WORKDIR/temp_sounds/ $PACK_WORKDIR/rootfs/res/sounds/

script_config
update_boot
do_fs
do_mbr
do_fes1_uboot_update
do_create_rtos_full_img
do_package
do_finish
