ccflags-y  =

usrlibs-y  +=	--start-group \
		--end-group
MOD_NAME := slib
SUF_NAME := mod
PRJ_PATH := $(prjtree)/ramfs/mod/
TEMP_PATH := $(srctree)/emodules/staging

objy += api_entry.o
objy += mod_slib.o
objy += slib/slib_stdlib.o
objy += slib/slib_string.o
objy += slib/sprintf/_fpmaxtostr.o
objy += slib/sprintf/_store_inttype.o
objy += slib/sprintf/_uintmaxtostr.o
objy += slib/sprintf/vfprintf.o
objy += slib/sprintf/vsnprintf.o
objy += magic.o
$(MOD_NAME)-objs := $(objy)

include $(MELIS_BASE)/scripts/Makefile.mods
