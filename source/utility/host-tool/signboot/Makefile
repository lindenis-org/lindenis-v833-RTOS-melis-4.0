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

DESTINATION := checksum
LIBS :=
INCLUDES := .

RM := rm -rf

# GNU Make的隐含变量定义
CC         = gcc -static -m32
CXX        = g++ -static -m32
CFLAGS     = -g -Wall -Os -pipe
CFLAGS    += $(addprefix -I,$(INCLUDES))
CFLAGS    += -MMD
CXXFLAGS   = $(CFLAGS)
LDFLAGS	   = -s -Wl,--as-needed
STRIP=strip

#以下部分无需修改
SRCS   := $(wildcard *.c) $(wildcard *.cc) $(wildcard *.cpp)
OBJS   := $(patsubst %.cpp, %.o, $(patsubst %.cc, %.o, $(patsubst %.c, %.o, $(SRCS))))
DEPS   := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SRCSS := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS))) $(wildcard $(patsubst %.d,%.cc,$(MISSING_DEPS))) $(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS)))

.PHONY: all deps objs clean rebuild

all: $(DESTINATION)

deps: $(DEPS)
	$(CXX) -MM -MMD $(SRCS)

objs: $(OBJS)

clean:
	$(RM) *.o
	$(RM) *.d
	$(RM) $(DESTINATION)

rebuild: clean all

### Build Rules

%.o: %.c
	@echo "---> compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cc
	@echo "---> compiling $< ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "---> compiling $< ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS):
	@$(RM) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(DESTINATION): $(OBJS)
	$(CXX) -o $(DESTINATION) $(OBJS) $(addprefix -l,$(LIBS))
	@$(STRIP) $(DESTINATION)
	@cp $(DESTINATION) ../../../workspace/suniv/beetles/packtool/$(DESTINATION)
