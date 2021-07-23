/*
 * ===========================================================================================
 *
 *       Filename:  hal_sem.c
 *
 *    Description:  memory allocator impl for hal driver.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-27 14:20:53
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-12-16 16:55:24
 *
 * ===========================================================================================
 */

#include <hal_mem.h>
#include <rtdef.h>
#include <log.h>

void *hal_malloc(uint32_t size)
{
    void *rt_malloc(uint32_t size);
    return rt_malloc(size);
}

void hal_free(void *p)
{
    void rt_free(void *rmem);
    rt_free(p);
}

