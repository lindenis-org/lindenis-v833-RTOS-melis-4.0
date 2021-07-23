ccflags-y += 	-I$(srctree)/ekernel/subsys/avframework/v4l2/include \
		-I$(srctree)/ekernel/subsys/avframework/v4l2/include/uapi \
		-I$(srctree)/ekernel/drivers/hal/source/osal/include \
		-I$(srctree)/ekernel/subsys/avframework/v4l2/include/media \
		-I$(srctree)/ekernel/drivers/hal/source/ccmu \
		-I$(srctree)/ekernel/drivers/include/hal

obj-$(CONFIG_VIN_SENSOR_imx386)			+= imx386_mipi.o
obj-$(CONFIG_VIN_SENSOR_C2398)			+= C2398_mipi.o
obj-$(CONFIG_VIN_SENSOR_gc2053)			+= gc2053_mipi.o
obj-$(CONFIG_VIN_SENSOR_tw2866)			+= tw2866.o
obj-$(CONFIG_VIN_SENSOR_imx307) 		+= imx307_mipi.o
obj-$(CONFIG_VIN_SENSOR_bt656_bt1120_test) 		+= bt656_bt1120_test.o
obj-y += sensor_helper.o
