/*
 * ===========================================================================================
 *
 *       Filename:  hal_atomic.h
 *
 *    Description:  header files to define atomic operation for hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-25 20:50:36
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-26 15:31:05
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_ATOMIC_H
#define SUNXI_HAL_ATOMIC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <arch.h>

uint32_t hal_spin_lock_irqsave(void);
void hal_spin_unlock_irqrestore(uint32_t __cpsr);

#ifdef __cplusplus
}
#endif
#endif
