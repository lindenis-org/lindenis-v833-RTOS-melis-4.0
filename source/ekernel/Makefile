include $(MELIS_BASE)/scripts/Makefile.common
OBJCOPYFLAGS     := 	-S -O binary \
			-R .note \
			-R .comment \
			-R COMMON \
			-R .ARM.attributes \
			-R .en_frame \
			-R .init -R .fini \
			--gap-fill=0xff

subdir-ccflags-y +=  	-I$(srctree)/include/melis/kernel \
			-I$(srctree)/include/melis/module \
			-I$(srctree)/ekernel/subsys/finsh_cli \
			-I$(srctree)/ekernel/core/rt-thread/include \
			-I$(srctree)/ekernel/subsys/thirdparty/dfs/include \
			-I$(srctree)/ekernel/core/rt-thread/include/libc
subdir-ccflags-y += -DFD_SETSIZE=1024

subdir-ccflags-y += -I$(srctree)/include/melis/common

MOD_NAME 		:= melis30
$(MOD_NAME)-objs 	+= built-in.o
obj-m 			+= $(MOD_NAME).o

obj-y 		        := arch/ core/ drivers/ subsys/
obj-${CONFIG_MELIS_VIRTUAL_FILESYSTEM} += filesystem/

usrlibs-y  	        += -L$(srctree)/${elibrary-libs} \
			   --start-group \
			   --end-group 

usrlibs-${CONFIG_VIDEO_V4L2} 	+= --require-defined=vin_init
usrlibs-${CONFIG_COMPONENT_ADBD}+= --require-defined=adbd_init

ifneq ($(filter y, $(CONFIG_mpp_venc) $(CONFIG_mpp_vdec)),)
usrlibs-y += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/base \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/library \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/library/${LIB_DIR} \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/memory \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/vdecoder \
	--start-group \
	-lcdc_base \
	-lVE \
	-lMemAdapter \
	--end-group
endif

usrlibs-$(CONFIG_mpp_venc) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/base \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/library \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/library/${LIB_DIR} \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/memory \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/vdecoder \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/vencoder \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/vencoder/base \
	--start-group \
	-lvencoder \
	-lvenc_base \
	-lvenc_codec \
	--end-group

usrlibs-$(CONFIG_mpp_vdec) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/base \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/library \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/library/${LIB_DIR} \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/memory \
	-L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarc/vdecoder \
	--start-group \
	-lawh264 \
	-lawh265 \
	-lawmjpegplus \
	-lvideoengine \
	-lvdecoder \
	--end-group

usrlibs-$(CONFIG_mpp_vi) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libisp/out \
	--start-group \
	-liniparser \
	-lisp_algo \
	-lisp_dev \
	-lisp_ini \
	-lisp_math \
	--end-group

usrlibs-$(CONFIG_mpp_aenc) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/AudioLib/lib \
	--start-group \
	-laacenc \
	-lmp3enc \
	--end-group

usrlibs-$(CONFIG_mpp_adec) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/AudioLib/lib \
	--start-group \
	-laac \
	-lmp3 \
	-lwav \
	--end-group

usrlibs-$(CONFIG_mpp_aec) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/aec_lib \
	--start-group \
	-lAec \
	--end-group

usrlibs-$(CONFIG_mpp_softdrc) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/drc_lib \
	--start-group \
	-lDrc \
	--end-group

usrlibs-$(CONFIG_mpp_ai_agc) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/agc_lib \
	--start-group \
	-lAgc \
	--end-group

usrlibs-$(CONFIG_mpp_ans_libwebrtc) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/ans_lib/libwebrtc \
	--start-group \
	-lAns \
	--end-group

usrlibs-$(CONFIG_mpp_ans_liblstm) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/ans_lib/liblstm \
	--start-group \
	-lAns \
	--end-group

usrlibs-$(CONFIG_mpp_ise_gdc) += -L$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libISE \
	--start-group \
	-l_ise_gdc \
	--end-group

usrlibs-$(CONFIG_AP6203) += -L$(srctree)/ekernel/drivers/drv/source/net/brcm_wifi_libs/libs \
	--start-group \
	-lbrcm \
	--end-group

usrlibs-$(CONFIG_XR819) += -L$(srctree)/ekernel/drivers/drv/source/net/xradio/ \
	--start-group \
	-lxr819 \
	--end-group

usrlibs-$(CONFIG_XR829) += -L$(srctree)/ekernel/drivers/drv/source/net/xradio/ \
	--start-group \
	-lxr829 \
	--end-group

lnkflags-y 		+= --omagic --gc-sections -cref --entry=_start -p --no-undefined \
    			   -X --pic-veneer -T$(obj)/arch/arm/lds/kernel.lds \
			   -Map=$(obj)/linker.map --build-id=sha1

$(obj)/$(MOD_NAME).bin :%.bin :%.o FORCE
	$(call if_changed,objcopy)
	$(call if_changed,renameelf)
	$(Q)cp -f $@  $(prjtree)/beetles/epos.img
	$(Q)$(OBJDUMP) -d $(<:.o=.elf) > $(obj)/$(MOD_NAME).dis
	$(Q)mksysmap $(<:.o=.elf) $(obj)/System.map
	$(Q)echo -e '\033[0;31;1m'
	$(Q)$(SIZE)  $(<:.o=.elf)
	$(Q)echo -e '\033[0m'

always     	:= $(MOD_NAME).bin
