# SPDX-License-Identifier: GPL-2.0+

include $(TOPDIR)/mk/config.mk

LIB	:= $(obj)libmain.o

HEAD    := boot0_head.o
MAIN   += boot0_main.o
COBJS   += load_image.o

SRCS	:= $(MAIN:.o=.c) $(COBJS:.o=.c) $(HEAD:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS) $(COBJS-y) $(SOBJS))
HEAD    := $(addprefix $(obj),$(HEAD))
MAIN	:= $(addprefix $(obj),$(MAIN))

all:	$(obj).depend $(HEAD) $(MAIN) $(LIB) $(DATA)

$(LIB):	$(OBJS) $(HEAD) $(MAIN)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(TOPDIR)/mk/rules.mk

sinclude $(obj).depend

#########################################################################
