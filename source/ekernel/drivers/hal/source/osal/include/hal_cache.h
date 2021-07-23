/*
 * ===========================================================================================
 *
 *       Filename:  hal_timer.h
 *
 *    Description:  header files to define osal timer for hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-25 20:50:36
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-12-13 15:22:47
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_CACHE_H
#define SUNXI_HAL_CACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <arch.h>

void cpu_dcache_clean(unsigned long vaddr_start, unsigned long size);
void cpu_dcache_clean_invalidate(unsigned long vaddr_start, unsigned long size);
void cpu_dcache_invalidate(unsigned long vaddr_start, unsigned long size);
void cpu_icache_invalidate_all(void);
void cpu_dcache_clean_all(void);

#ifdef __cplusplus
}
#endif
#endif
