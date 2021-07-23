subdir-ccflags-y +=     -I$(srctree)/include/melis/kernel  \
                        -I$(srctree)/include/melis/module  \
                        -I$(srctree)/include/melis/common  \
                        -I$(srctree)/include/melis/kernel/drivers \
                        -I$(srctree)/include/melis/kernel/pthread
obj-y += exec.o
obj-y += ksrv.o
obj-${CONFIG_MELIS_MODULE} += loader.o
obj-${CONFIG_MELIS_MODULE} += mods.o
obj-y += swi.o
obj-y += az100/
obj-y += bmp/
obj-${CONFIG_RT_SUPPORT_OPENOCD} += openocd.o
obj-${CONFIG_RT_USING_TASK_PERF_MONITOR} += perf.o
obj-${CONFIG_PTHREAD} += pthread_swi.o
