#=====================================================================================
#
#      Filename:  Makefile
#
#   Description:  General Makefile for host tools.
#
#       Version:  2.0
#        Create:  2017-11-10 11:06:11
#      Revision:  none
#      Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
#
#        Author:  caozilong@allwinnertech.com
#  Organization:  BU1-PSW
# Last Modified:  2017-11-10 11:06:11
#
#=====================================================================================

DESTINATION := fix_file
LIBS :=
INCLUDES := .

RM := rm -f

# GNU Make的隐含变量定义
CC=gcc -static
CFLAGS  = -g -Wall -Os
CFLAGS += $(addprefix -I,$(INCLUDES))
CFLAGS += -MMD
STRIP=strip

#以下部分无需修改
SRCS   := $(wildcard *.c)
OBJS   := $(patsubst %.c,%.o,$(SRCS))
DEPS   := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SRCSS := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS)))

.PHONY: all deps objs clean rebuild

all: $(DESTINATION)

deps: $(DEPS)
	$(CC) -MM -MMD $(SRCS)

objs: $(OBJS)

clean:
	$(RM) *.o
	$(RM) *.d
	$(RM) $(DESTINATION)

rebuild: clean all

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS):
	@$(RM) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(DESTINATION): $(OBJS)
	$(CC) -o $(DESTINATION) $(OBJS) $(addprefix -l,$(LIBS))
	@$(STRIP) $(DESTINATION)
	@cp $(DESTINATION) ../../../workspace/suniv/beetles/packtool/$(DESTINATION)
