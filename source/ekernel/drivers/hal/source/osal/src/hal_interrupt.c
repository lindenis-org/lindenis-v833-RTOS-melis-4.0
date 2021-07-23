/*
 * ===========================================================================================
 *
 *       Filename:  interrupt.c
 *
 *    Description:  interrupt osal impl. for hal layer.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-25 20:34:50
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-26 10:07:30
 *
 * ===========================================================================================
 */

#include <interrupt.h>
#include <rtthread.h>

uint32_t hal_interrupt_get_nest(void)
{
    uint32_t nest = rt_interrupt_get_nest();
    return nest;
}
