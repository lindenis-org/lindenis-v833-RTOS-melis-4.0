#
# SPDX-License-Identifier: GPL-2.0+
# (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
# Configuration settings for the Allwinner A80 (sun9i) CPU
#

include $(TOPDIR)/mk/config.mk
LIB	=  libboard.o


COBJS	+= board.o
COBJS	+= clock.o

SRCS	:= $(COBJS:.o=.c) $(SOBJS:.o=.S)
OBJS	:= $(addprefix $(obj),$(COBJS) $(SOBJS))

all:	 $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(TOPDIR)/mk/rules.mk

sinclude .depend

#########################################################################
