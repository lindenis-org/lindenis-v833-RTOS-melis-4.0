/*
 * =====================================================================================
 *
 *       Filename:  sys_charset.h
 *
 *    Description:  charset header.
 *
 *        Version:  Melis3.0
 *         Create:  2018-01-19 15:45:43
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-01-19 15:46:05
 *
 * =====================================================================================
 */
#ifndef __SYS_CHARSET__
#define __SYS_CHARSET__

__u32 esCHS_GetChUpperTbl(__s32 type, void *buf, __u32 size);
__u32 esCHS_GetChLowerTbl(__s32 charset_type, void *buf, __u32 size);
__s32 esCHS_Uni2Char(__s32 type, __u16 uni, __u8 *str, __u32 len);
__s32 esCHS_Char2Uni(__s32 type, const __u8 *str, __u32 len, __u16 *uni);
__s32 eschs_init(__u32 mode, void *p_arg);

#endif
