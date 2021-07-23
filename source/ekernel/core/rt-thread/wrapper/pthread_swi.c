/*
 * ===========================================================================================
 *
 *       Filename:  swi.c
 *
 *    Description:  syscall entry definition.
 *
 *        Version:  Melis3.0
 *         Create:  2018-04-24 11:11:07
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-01-14 13:05:16
 *
 * ===========================================================================================
 */

#include <typedef.h>
#include <rtthread.h>
#include <kconfig.h>
#include <mod_defs.h>
#include <const.h>
#include <rthw.h>
#include <kapi.h>
#include <debug.h>
#include <log.h>

#include "pthread.h"
#include "semaphore.h"

const uint32_t SWIHandler_PTHREAD[] =
{
    (uint32_t)pthread_attr_destroy,
    (uint32_t)pthread_attr_init,
    (uint32_t)pthread_attr_setdetachstate,
    (uint32_t)pthread_attr_getdetachstate,
    (uint32_t)pthread_attr_setschedpolicy,
    (uint32_t)pthread_attr_getschedpolicy,
    (uint32_t)pthread_attr_setschedparam,
    (uint32_t)pthread_attr_getschedparam,
    (uint32_t)pthread_attr_setstacksize,
    (uint32_t)pthread_attr_getstacksize,
    (uint32_t)pthread_attr_setstackaddr,
    (uint32_t)pthread_attr_getstackaddr,
    (uint32_t)pthread_attr_setstack,
    (uint32_t)pthread_attr_getstack,
    (uint32_t)pthread_attr_setguardsize,
    (uint32_t)pthread_attr_getguardsize,
    (uint32_t)pthread_attr_setscope,
    (uint32_t)pthread_attr_getscope,
    (uint32_t)pthread_system_init,
    (uint32_t)pthread_create,
    (uint32_t)pthread_detach,
    (uint32_t)pthread_join,
    (uint32_t)pthread_exit,
    (uint32_t)pthread_once,
    (uint32_t)pthread_cleanup_pop,
    (uint32_t)pthread_cleanup_push,
    (uint32_t)pthread_cancel,
    (uint32_t)pthread_testcancel,
    (uint32_t)pthread_setcancelstate,
    (uint32_t)pthread_setcanceltype,
    (uint32_t)pthread_atfork,
    (uint32_t)pthread_kill,
    (uint32_t)pthread_self,
    (uint32_t)pthread_mutex_init,
    (uint32_t)pthread_mutex_destroy,
    (uint32_t)pthread_mutex_lock,
    (uint32_t)pthread_mutex_unlock,
    (uint32_t)pthread_mutex_trylock,
    (uint32_t)pthread_mutexattr_init,
    (uint32_t)pthread_mutexattr_destroy,
    (uint32_t)pthread_mutexattr_gettype,
    (uint32_t)pthread_mutexattr_settype,
    (uint32_t)pthread_mutexattr_setpshared,
    (uint32_t)pthread_mutexattr_getpshared,
    (uint32_t)pthread_condattr_destroy,
    (uint32_t)pthread_condattr_init,
    (uint32_t)pthread_condattr_getclock,
    (uint32_t)pthread_condattr_setclock,
    (uint32_t)pthread_cond_init,
    (uint32_t)pthread_cond_destroy,
    (uint32_t)pthread_cond_broadcast,
    (uint32_t)pthread_cond_signal,
    (uint32_t)pthread_cond_wait,
    (uint32_t)pthread_cond_timedwait,
    (uint32_t)pthread_rwlockattr_init,
    (uint32_t)pthread_rwlockattr_destroy,
    (uint32_t)pthread_rwlockattr_getpshared,
    (uint32_t)pthread_rwlockattr_setpshared,
    (uint32_t)pthread_rwlock_init,
    (uint32_t)pthread_rwlock_destroy,
    (uint32_t)pthread_rwlock_rdlock,
    (uint32_t)pthread_rwlock_tryrdlock,
    (uint32_t)pthread_rwlock_timedrdlock,
    (uint32_t)pthread_rwlock_timedwrlock,
    (uint32_t)pthread_rwlock_unlock,
    (uint32_t)pthread_rwlock_wrlock,
    (uint32_t)pthread_rwlock_trywrlock,
    (uint32_t)pthread_spin_init,
    (uint32_t)pthread_spin_destroy,
    (uint32_t)pthread_spin_lock,
    (uint32_t)pthread_spin_trylock,
    (uint32_t)pthread_spin_unlock,
    (uint32_t)pthread_barrierattr_destroy,
    (uint32_t)pthread_barrierattr_init,
    (uint32_t)pthread_barrierattr_getpshared,
    (uint32_t)pthread_barrierattr_setpshared,
    (uint32_t)pthread_barrier_destroy,
    (uint32_t)pthread_barrier_init,
    (uint32_t)pthread_barrier_wait,
    (uint32_t)pthread_getspecific,
    (uint32_t)pthread_setspecific,
    (uint32_t)pthread_key_create,
    (uint32_t)pthread_key_delete,
    (uint32_t)sem_close,
    (uint32_t)sem_destroy,
    (uint32_t)sem_getvalue,
    (uint32_t)sem_init,
    (uint32_t)sem_open,
    (uint32_t)sem_post,
    (uint32_t)sem_timedwait,
    (uint32_t)sem_trywait,
    (uint32_t)sem_unlink,
    (uint32_t)sem_wait
};
