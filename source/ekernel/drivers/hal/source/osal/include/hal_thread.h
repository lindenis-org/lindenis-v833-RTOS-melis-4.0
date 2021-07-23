/*
 * ===========================================================================================
 *
 *       Filename:  hal_thread.h
 *
 *    Description:  header files to define thread for hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-25 20:50:36
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-03-26 18:00:48
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_OSAL_H
#define SUNXI_HAL_OSAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>

#define HAL_THREAD_STACK_SIZE    (0x2000)
#define HAL_THREAD_PRIORITY      (    15)
#define HAL_THREAD_TIMESLICE     (    10)

void *kthread_create(void (*threadfn)(void *data), void *data, const char *namefmt, ...);
int kthread_stop(void *thread);
int kthread_start(void *thread);
void udelay(unsigned int us);
int kthread_wakeup(void *thread);
int kthread_suspend(void *thread);

int msleep(unsigned int msecs);

/**
 * kthread_run - create and wake a thread.
 * @threadfn: the function to run until signal_pending(current).
 * @data: data ptr for @threadfn.
 * @namefmt: printf-style name for the thread.
 *
 * Description: Convenient wrapper for kthread_create() followed by
 * wake_up_process().  Returns the kthread or ERR_PTR(-ENOMEM).
 */
#define kthread_run(threadfn, data, namefmt, ...)			   \
({									   \
	struct rt_thread *__k						   \
		= kthread_create(threadfn, data, namefmt, ## __VA_ARGS__); \
	if (__k != NULL)				   \
		rt_thread_startup(__k);					   \
	__k;								   \
})

//#define in_interrupt(...)   rt_interrupt_get_nest()
#ifdef __cplusplus
}
#endif
#endif
