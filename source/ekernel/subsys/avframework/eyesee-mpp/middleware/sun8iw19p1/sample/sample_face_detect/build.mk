#
# 1. Set the path and clear environment
#   TARGET_PATH := $(call my-dir)
#   include $(ENV_CLEAR)
#
# 2. Set the source files and headers files
#   TARGET_SRC := xxx_1.c xxx_2.c
#   TARGET_INc := xxx_1.h xxx_2.h
#
# 3. Set the output target
#   TARGET_MODULE := xxx
#
# 4. Include the main makefile
#   include $(BUILD_BIN)
#
# Before include the build makefile, you can set the compilaion
# flags, e.g. TARGET_ASFLAGS TARGET_CFLAGS TARGET_CPPFLAGS
#

TARGET_PATH :=$(call my-dir)
TOP_PATH := $(TARGET_PATH)

# ============================================================================
# start gcc
#########################################
include $(ENV_CLEAR)
TARGET_SRC := \
    sample_face_detect.c \
    osd_helper.c \
    mpp_eve.c \
    mpp_vi.c \
    mpp_vo.c \
    mpp_sys.c \
    mpp_venc.c \
    mpp_osd.c \

TARGET_INC := \
    $(TARGET_TOP)/system/include \
    $(TARGET_TOP)/system/public/include/kernel-headers \
    $(TARGET_TOP)/middleware/include/utils \
    $(TARGET_TOP)/middleware/include/media \
    $(TARGET_TOP)/middleware/include \
    $(TARGET_TOP)/middleware/media/include \
    $(TARGET_TOP)/middleware/media/include/utils \
    $(TARGET_TOP)/middleware/media/include/component \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/include/V4l2Camera \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/isp_tuning \
    $(TARGET_TOP)/middleware/media/LIBRARY/libAIE_Vda/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/include_stream \
    $(TARGET_TOP)/middleware/media/LIBRARY/libcedarc/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/libeveface/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/include_ai_common \
    $(TARGET_TOP)/middleware/media/LIBRARY/include_eve_common \

TARGET_SHARED_LIB := \
    libpthread \
    liblog \
    libMemAdapter \
    libvdecoder \
    libmedia_utils \
    libmedia_mpp \
    libmpp_vi \
    libmpp_vo \
    libmpp_isp \
    libmpp_ise \
    libmpp_component \
    libmpp_uvc \
    libsample_confparser \
    libISP \
    libeve_event \

TARGET_STATIC_LIB :=

TARGET_CPPFLAGS += -fPIC -Wall -Wno-unused-but-set-variable
TARGET_CFLAGS += -fPIC -Wall -Wno-unused-but-set-variable

#TARGET_LDFLAGS += -static

TARGET_MODULE := sample_face_detect

include $(BUILD_BIN)
