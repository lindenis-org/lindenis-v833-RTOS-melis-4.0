#
# (C) Copyright 2000-2011
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2011
# Daniel Schwierzeck, daniel.schwierzeck@googlemail.com.
#
# (C) Copyright 2011
# Texas Instruments Incorporated - http://www.ti.com/
# Aneesh V <aneesh@ti.com>
#
# This file is released under the terms of GPL v2 and any later version.
# See the file COPYING in the root directory of the source tree for details.
#
# Based on top-level Makefile.
#

SKIP_AUTO_CONF:=yes
include $(TOPDIR)/mk/config.mk
include $(TOPDIR)/.module.common.mk
include $(TOPDIR)/board/$(PLATFORM)/$(MAKECMDGOALS).mk

#This for Tina system, to copy bin to tina SDK
ifeq ($(TARGET_BUILD_VARIANT),tina)
tina_path=$(TOPDIR)/../../../target/allwinner/$(TARGET_PLATFORM)-common/bin
exist = $(shell if [ -d $(tina_path) ]; then echo "exist"; else echo "notexist"; fi;)
ifeq ($(exist),exist)
TARGETDIR=$(TOPDIR)/../../../target/allwinner/$(TARGET_PLATFORM)-common/bin
endif

tina_path=$(TOPDIR)/../../../device/config/chips/$(TARGET_PLATFORM)/bin
exist = $(shell if [ -d $(tina_path) ]; then echo "exist"; else echo "notexist"; fi;)
ifeq ($(exist),exist)
TARGETDIR=$(TOPDIR)/../../../device/config/chips/$(TARGET_PLATFORM)/bin
endif

else ifeq ($(TARGET_BUILD_VARIANT),rtos)
rtos_path=$(TOPDIR)/../../../board/bin
exist = $(shell if [ -d $(rtos_path) ]; then echo "exist"; else echo "notexist"; fi;)
ifeq ($(exist),exist)
TARGETDIR=$(rtos_path)
endif

else
#TOOLS_DIR  := $(TOPDIR)/../../../device/config
#TARGETDIR=$(TOPDIR)/../../../device/config/chips/$(PLATFORM)/bin

ifneq ($(LICHEE_BUSSINESS), )
	TARGETDIR=$(LICHEE_CHIP_CONFIG_DIR)/$(LICHEE_BUSSINESS)/bin
else
	TARGETDIR=$(LICHEE_CHIP_CONFIG_DIR)/bin
endif
endif

CPUDIR = $(TOPDIR)/arch/$(ARCH)/cpu/$(CPU)
BOOT0_LDSCRIPT := $(CPUDIR)/boot0.lds

# We want the final binaries in this directory
NBOOT_DIR := $(TOPDIR)/nboot/
obj := $(TOPDIR)/nboot/

LIBS-y += arch/$(ARCH)/cpu/$(CPU)/libarch.o
LIBS-y += nboot/main/libmain.o
LIBS-y += drivers/libdrivers.o
LIBS-y += common/libcommon.o
LIBS-y += board/$(PLATFORM)/libboard.o

LIBS := $(addprefix $(TOPDIR)/,$(sort $(LIBS-y)))

COBJS-y += $(TOPDIR)/nboot/main/boot0_main.o

OBJS = $(sort $(COBJS-y))

ifndef CONFIG_BOOT0_SIZE_LIMIT
	BOOT0SIZE = $(CFG_SYS_INIT_RAM_SIZE)
else
	BOOT0SIZE = $(CFG_BOOT0_SIZE_LIMIT)
endif


# Special flags for CPP when processing the linker script.
# Pass the version down so we can handle backwards compatibility
# on the fly.
LDPPFLAGS += \
	-DBOOT0ADDR=$(CFG_BOOT0_RUN_ADDR)	 \
	-DBOOT0SIZE=$(BOOT0SIZE)	 \
	-DCPUDIR=$(CPUDIR) \
	$(shell $(LD) --version | \
	  sed -ne 's/GNU ld version \([0-9][0-9]*\)\.\([0-9][0-9]*\).*/-DLD_MAJOR=\1 -DLD_MINOR=\2/p')

spinor:	 $(LIBS) $(LIBSPINOR) boot0.lds
	$(Q)echo bootaddr is $(CFG_BOOT0_RUN_ADDR)
	$(LD) -r -o lib$(PLATFORM)_spinor.o $(LIBS)
	$(LD) lib$(PLATFORM)_spinor.o $(OBJS) $(LIBSPINOR) $(PLATFORM_LIBGCC) $(LDFLAGS) $(LDFLAGS_GC) -Tboot0.lds -o boot0_spinor.elf -Map boot0_spinor.map
	$(OBJCOPY) $(OBJCFLAGS) -O binary  boot0_spinor.elf boot0_spinor.bin
	@$(TOPDIR)/mk/gen_check_sum $(SRCTREE)/nboot/boot0_spinor.bin boot0_spinor_$(PLATFORM)$(DRAM_TYPE_NAME).bin > /dev/null
	@$(TOPDIR)/mk/encrypto_boot0 -f boot0_spinor_$(PLATFORM)$(DRAM_TYPE_NAME).bin  -c $(LICHEE_IC) >/dev/null
	$(STRIP) -g $(TOPDIR)/nboot/lib$(PLATFORM)_spinor.o
	$(CP) $(TOPDIR)/nboot/lib$(PLATFORM)_spinor.o $(TOPDIR)/../spl-pub/board/$(PLATFORM)/lib$(PLATFORM)_spinor.a
	#@-cp -v boot0_spinor_$(PLATFORM)$(DRAM_TYPE_NAME).bin $(TARGETDIR)/

mmc:	 $(LIBS) $(LIBMMC) boot0.lds
	$(Q)echo bootaddr is $(CFG_BOOT0_RUN_ADDR)
	$(LD) -r -o lib$(PLATFORM)_sdcard.o $(LIBS)
	$(LD) lib$(PLATFORM)_sdcard.o $(OBJS) $(LIBMMC) $(PLATFORM_LIBGCC) $(LDFLAGS) $(LDFLAGS_GC) -Tboot0.lds -o boot0_sdcard.elf -Map boot0_sdcard.map
	$(OBJCOPY) $(OBJCFLAGS) -O binary  boot0_sdcard.elf boot0_sdcard.bin
	@$(TOPDIR)/mk/gen_check_sum boot0_sdcard.bin boot0_sdcard_$(PLATFORM)$(DRAM_TYPE_NAME).bin > /dev/null
	@$(TOPDIR)/mk/encrypto_boot0 -f boot0_sdcard_$(PLATFORM)$(DRAM_TYPE_NAME).bin  -c $(LICHEE_IC) >/dev/null
	$(STRIP) -g $(TOPDIR)/nboot/lib$(PLATFORM)_sdcard.o
	$(CP) $(TOPDIR)/nboot/lib$(PLATFORM)_sdcard.o $(TOPDIR)/../spl-pub/board/$(PLATFORM)/lib$(PLATFORM)_sdcard.a
	#@-cp -v boot0_sdcard_$(PLATFORM)$(DRAM_TYPE_NAME).bin $(TARGETDIR)/

nand:	 $(LIBS) $(LIBNAND) boot0.lds
	$(Q)echo bootaddr is $(CFG_BOOT0_RUN_ADDR)
	$(LD) -r -o lib$(PLATFORM)_nand.o $(LIBS)
	$(LD) lib$(PLATFORM)_nand.o $(OBJS) $(LIBNAND) $(PLATFORM_LIBGCC) $(LDFLAGS) $(LDFLAGS_GC) -Tboot0.lds -o boot0_nand.elf -Map boot0_nand.map
	$(OBJCOPY) $(OBJCFLAGS) -O binary  boot0_nand.elf boot0_nand.bin
	@$(TOPDIR)/mk/gen_check_sum boot0_nand.bin boot0_nand_$(PLATFORM)$(DRAM_TYPE_NAME).bin > /dev/null
	@$(TOPDIR)/mk/encrypto_boot0 -f boot0_nand_$(PLATFORM)$(DRAM_TYPE_NAME).bin  -c $(LICHEE_IC) >/dev/null
	$(STRIP) -g $(TOPDIR)/nboot/lib$(PLATFORM)_nand.o
	$(CP) $(TOPDIR)/nboot/lib$(PLATFORM)_nand.o $(TOPDIR)/../spl-pub/board/$(PLATFORM)/lib$(PLATFORM)_nand.a
	#@-cp -v boot0_nand_$(PLATFORM)$(DRAM_TYPE_NAME).bin $(TARGETDIR)/


mmc_$(DRAM_TYPE):mmc
nand_$(DRAM_TYPE):nand

$(LIBS): depend
	$(MAKE) -C $(dir $(subst $(OBJTREE),,$@))

$(LIBNAND): depend
	$(MAKE) -C $(dir $(subst $(OBJTREE),,$@))

$(LIBMMC): depend
	$(MAKE) -C $(dir $(subst $(OBJTREE),,$@))

$(LIBSPINOR): depend
	$(MAKE) -C $(dir $(subst $(OBJTREE),,$@))

boot0.lds: $(BOOT0_LDSCRIPT) depend
	$(Q)$(CPP) $(ALL_CFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - <$(BOOT0_LDSCRIPT) >$@

$(NBOOT_DIR)cur.log:
	@git show HEAD --pretty=format:"%H" | head -n 1 > cur.log

sinclude $(TOPDIR)/mk/target_for_conf.mk
depend: .depend build-confs
#########################################################################

# defines $(NBOOT_DIR).depend target
include $(TOPDIR)/mk/rules.mk

sinclude .depend

#########################################################################
