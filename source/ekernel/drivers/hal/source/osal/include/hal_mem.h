/*
 * ===========================================================================================
 *
 *       Filename:  hal_sem.h
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
 *  Last Modified:  2019-12-17 10:14:06
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_MEM_H
#define SUNXI_HAL_MEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>
#include <stddef.h>
#include <stdint.h>

void *hal_malloc(uint32_t size);
void hal_free(void *p);
uint32_t awos_arch_virt_to_phys(uint32_t virtaddr);
uint32_t awos_arch_phys_to_virt(uint32_t phyaddr);

#define __va_to_pa(vaddr) awos_arch_virt_to_phys((vaddr))
#define __pa_to_va(paddr) awos_arch_phys_to_virt((paddr))

#ifdef __cplusplus
}
#endif
#endif
