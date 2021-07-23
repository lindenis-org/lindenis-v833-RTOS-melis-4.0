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
 *  Last Modified:  2019-11-26 20:22:49
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_TIMER_H
#define SUNXI_HAL_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

unsigned sleep(unsigned int __seconds);
/* int usleep(__useconds_t __useconds); */
int msleep(unsigned int msecs);

typedef rt_timer_t osal_timer_t;
typedef void (*timeout_func)(void *parameter);

#define OSAL_TIMER_FLAG_DEACTIVATED  RT_TIMER_FLAG_DEACTIVATED
#define OSAL_TIMER_FLAG_ACTIVATED    RT_TIMER_FLAG_ACTIVATED
#define OSAL_TIMER_FLAG_ONE_SHOT     RT_TIMER_FLAG_ONE_SHOT
#define OSAL_TIMER_FLAG_PERIODIC     RT_TIMER_FLAG_PERIODIC

#define OSAL_TIMER_FLAG_HARD_TIMER   RT_TIMER_FLAG_HARD_TIMER
#define OSAL_TIMER_FLAG_SOFT_TIMER   RT_TIMER_FLAG_SOFT_TIMER

#define OSAL_TIMER_CTRL_SET_TIME     RT_TIMER_CTRL_SET_TIME
#define OSAL_TIMER_CTRL_GET_TIME     RT_TIMER_CTRL_GET_TIME
#define OSAL_TIMER_CTRL_SET_ONESHOT  RT_TIMER_CTRL_SET_ONESHOT
#define OSAL_TIMER_CTRL_SET_PERIODIC RT_TIMER_CTRL_SET_PERIODIC
#define OSAL_TIMER_CTRL_GET_STATE    RT_TIMER_CTRL_GET_STATE

osal_timer_t osal_timer_create(const char *name,
                               timeout_func timeout,
                               void *parameter,
                               unsigned int time,
                               unsigned char flag);

int osal_timer_delete(osal_timer_t timer);
int osal_timer_start(osal_timer_t timer);
int osal_timer_stop(osal_timer_t timer);
int osal_timer_control(osal_timer_t timer, int cmd, void *arg);

#ifdef __cplusplus
}
#endif
#endif
