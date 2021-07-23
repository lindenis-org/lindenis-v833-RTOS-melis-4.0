/*
 * ===========================================================================================
 *
 *       Filename:  sys_hid.h
 *
 *    Description:  null
 *
 *        Version:  Melis3.0
 *         Create:  2018-09-05 16:19:45
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-09-05 16:19:52
 *
 * ===========================================================================================
 */

#ifndef SYS_HID_H
#define SYS_HID_H
#include <typedef.h>

extern __s32 esHID_SendMsg(__u32 msgid);
extern __s32 esHID_hiddevreg(__hdle hNode);
extern __s32 esHID_hiddevunreg(__hdle hNode, __u32 mode);

__s32 HID_Init(void);
__s32 HID_Exit(void);

#endif  /*SYS_HID_H*/
