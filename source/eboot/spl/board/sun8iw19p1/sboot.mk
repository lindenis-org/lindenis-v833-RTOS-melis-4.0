#
#config file for sun8iw18
#
#stroage
include $(TOPDIR)/board/sun8iw18p1/common.mk

MODULE=sboot

CFG_SUNXI_NAND =y
CFG_SUNXI_CE_20 =y
CFG_SUNXI_EFUSE =y

CFG_SUNXI_SBOOT =y
