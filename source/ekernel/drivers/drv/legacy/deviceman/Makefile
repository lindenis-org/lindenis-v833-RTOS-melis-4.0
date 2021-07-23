EXTRA_CFLAGS += -I$(obj)/ \
		-I$(obj)/devices   \
		-I$(obj)/devices/dmsdev \
		-I$(obj)/devices/hwscdev \
		-I$(obj)/devices/pwrman

obj-y += sys_device.o
obj-y += devices/dev.o
obj-y += devices/devman.o
obj-${CONFIG_LEGACY_DMS_DEV} += devices/dmsdev/dmsdev.o
obj-${CONFIG_LEGACY_HSWC_DEV} += devices/hwscdev/hwscdev.o
obj-${CONFIG_LEGACY_RAMDISK_DEV} += devices/ramdiskdev/ramdiskdev.o
obj-y += pwrman/dev_pwrman.o
