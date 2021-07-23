#ifndef HAL_MUTEX_H
#define HAL_MUTEX_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>
#include <stddef.h>
#include <stdint.h>

typedef rt_mutex_t hal_mutex_t;

int hal_mutex_create(hal_mutex_t *mutex);
int hal_mutex_delete(hal_mutex_t *mutex);
int hal_mutex_lock(hal_mutex_t * mutex);
int hal_mutex_unlock(hal_mutex_t * mutex);
int hal_mutex_trylock(hal_mutex_t * mutex);
int hal_mutex_timedwait(hal_mutex_t *mutex, int ticks);

#ifdef __cplusplus
}
#endif

#endif  /*HAL_MUTEX_H*/
