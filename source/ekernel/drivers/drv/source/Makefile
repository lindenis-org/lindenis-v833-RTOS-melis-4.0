subdir-ccflags-y := 	-I$(srctree)/include/melis/kernel  \
			-I$(srctree)/include/melis/module  \
			-I$(srctree)/include/melis/kernel/rtos  \
			-I$(srctree)/include/melis/kernel/drivers

subdir-ccflags-y += -I$(srctree)/ekernel/core/rt-thread/include

obj-y += serial/
obj-y += spi/
obj-y += spinor/
obj-y += gpio/
obj-y += timer/
obj-y += watchdog/
obj-y += rtc/
obj-y += net/xradio/
obj-y += net/brcm_wifi_libs/
obj-y += dma/
obj-y += sound/
obj-y += twi/
obj-y += pwm/
obj-y += usb/
obj-${CONFIG_DRIVER_SDMMC} += sdmmc/
obj-y += input/
obj-y += g2d_rcq/
obj-y += efuse/
obj-y += thermal/
obj-y += ce/
obj-y += eise/
obj-$(CONFIG_DISP2_SUNXI) += disp2/
obj-$(CONFIG_RAMFS_DEVICE) += ram_dev/
