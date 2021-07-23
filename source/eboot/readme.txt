1.Bootloader opensource for customilize.

目录：
spl: boot0代码
	|
	---fes:烧写fel相关
	|
	---nboot:非安全固件启动相关
	|
	---sboot:安全固件启动相关
	|
	---mk:编译规则和工具
	|
	---include:头文件
	|
	---arch:架构相关

tools:工具链

melis boot编译
一、编译启动用boot0文件
 1.cd spl-pub
 2.make distclean
 3.make p=sun8iw19p1_melis
 4.make boot0

编译完后会在 spl/nboot/ 目录下生成boot0_spinor_sun8iw19p1.bin文件
cp boot0_spinor_sun8iw19p1.bin文件到melis-v3.0/source/workspace/suniv/eGon/storage_media/spinor/目录下，并改名为boot0.bin
至此melis的启动用boot0.bin文件编译替换完成



二、编译烧写用fes1文件
 1.cd spl-pub
 2.make distclean
 3.make p=sun8iw19p1_melis
 4.make fes

编译完后会在 spl/fes/ 目录下生成fes1_sun8iw19p1.bin文件
cp fes1_sun8iw19p1.bin文件到melis-v3.0/source/workspace/suniv/eFex/usb_sun8iw19/目录下，并改名为fes1.bin
至此melis的usb烧写用fes1.bin文件编译替换完成

三、编译卡烧写用boot0文件
 1.cd spl-pub
 2.make distclean
 3.make p=sun8iw19p1_melis
 4.make boot0
编译完后会在 spl/nboot/ 目录下生成boot0_sdcard_sun8iw19p1.bin文件
cp boot0_sdcard_sun8iw19p1.bin文件到melis-v3.0/source/workspace/suniv/eFex/card_sun8iw19/目录下，并改名为boot0_sdcard.bin
至此melis的卡烧写用boot0_sdcard.bin文件编译替换完成
