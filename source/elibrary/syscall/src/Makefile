subdir-ccflags-y :=     -I$(srctree)/include/melis/kernel/drivers   \
			-I$(srctree)/include/melis/kernel/pthread   \
			-I$(srctree)/include/melis/arch/arm926      \
			-I$(srctree)/include/melis/module           \
			-I$(srctree)/include/melis/kernel

subdir-ccflags-y += -I$(srctree)/ekernel/core/rt-thread/include
subdir-ccflags-y += -I$(srctree)/ekernel/subsys/finsh_cli

lib-y += melis/syscall.o
lib-${CONFIG_ARMV7_A} += arch/arm/armv7a/syscall.o
lib-${CONFIG_ARMV5TE} += arch/arm/armv5te/syscall.o
lib-${CONFIG_PTHREAD} += pthread/syscall.o

TARGET := $(srctree)/${elibrary-libs}/libsyscall.a

include $(MELIS_BASE)/scripts/Makefile.rename

