/*
 * ===========================================================================================
 *
 *       Filename:  blobdata.S
 *
 *    Description:  include blob data to image.
 *
 *        Version:  Melis3.0 
 *         Create:  2019-11-21 18:03:56
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-08-25 10:46:44
 *
 * ===========================================================================================
 */
#include <kconfig.h>
.section .blobdata,#alloc
.globl  blob_input_ramdisk_start
blob_input_ramdisk_start:
/*    .incbin "workspace/suniv/ramfs/ramdisk.img"*/
.globl  blob_input_ramdisk_end
blob_input_ramdisk_end:

#ifdef CONFIG_OF
.globl  blob_input_dtb_start
blob_input_dtb_start:
    .incbin "workspace/suniv/ramfs/sunxi-v833.dtb"
.globl  blob_input_dtb_end
blob_input_dtb_end:
#endif

#ifdef CONFIG_FEXCONFIG
.globl  blob_input_fexconfig_start
blob_input_fexconfig_start:
    //.incbin "workspace/suniv/ramfs/sys_config.bin"
    .incbin "ekernel/arch/boot/fex/sys_config.bin"
.globl  blob_input_fexconfig_end
blob_input_fexconfig_end:
#endif
.end
