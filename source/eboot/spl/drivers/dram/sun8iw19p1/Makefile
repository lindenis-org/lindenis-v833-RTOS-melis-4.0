#
# SPDX-License-Identifier: GPL-2.0+
# (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
# Configuration settings for the Allwinner A80 (sun9i) CPU
#

include $(TOPDIR)/mk/config.mk

all:
ifeq ($(notdir $(shell find ./ -name lib-dram)), lib-dram)
	make -C lib-dram
else
	@echo "libdram exist"
endif

ifeq ($(notdir $(shell find ./ -name lib-chipid)), lib-chipid)
	make -C lib-chipid
else
	@echo "lib-chipid exist"
endif


#########################################################################

depend:	$(obj).depend
.PHONY: depend

# defines $(obj).depend target
include $(TOPDIR)/mk/rules.mk

#########################################################################
