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
 *  Last Modified:  2019-11-27 18:00:14
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_SEM_H
#define SUNXI_HAL_SEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>
#include <stddef.h>
#include <stdint.h>

typedef rt_sem_t hal_sem_t;

int hal_sem_create(hal_sem_t *sem, unsigned int cnt);
int hal_sem_delete(hal_sem_t);
int hal_sem_getvalue(hal_sem_t, int *);
int hal_sem_post(hal_sem_t);
int hal_sem_timedwait(hal_sem_t, int ticks);
int hal_sem_trywait(hal_sem_t);
int hal_sem_wait(hal_sem_t);

#ifdef __cplusplus
}
#endif
#endif
