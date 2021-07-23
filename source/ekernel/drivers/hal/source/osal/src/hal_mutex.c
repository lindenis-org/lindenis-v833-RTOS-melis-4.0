#include <stdlib.h>
#include <hal_mutex.h>

#define PEND_TICK_MAX (0x7FFFFFFF - 1)
#define HAL_MUTEX_OK 0

int hal_mutex_create(hal_mutex_t *mutex)
{
    void* res;
    if (!mutex)
    {
        return -1;
    }

    res = (void*)rt_mutex_create("hal_mutex", RT_IPC_FLAG_FIFO);
    if (res == NULL)
    {
        return -2;
    }
    *mutex = (hal_mutex_t)res;
    return HAL_MUTEX_OK;
}

int hal_mutex_delete(hal_mutex_t *mutex)
{
    if (!mutex)
    {
        return -1;
    }
    return rt_mutex_delete(*mutex);
}

int hal_mutex_lock(hal_mutex_t *mutex)
{
    if (!mutex)
    {
        return -1;
    }
    return rt_mutex_take(*mutex, PEND_TICK_MAX);
}

int hal_mutex_unlock(hal_mutex_t *mutex)
{
    if (!mutex)
    {
        return -1;
    }
    return rt_mutex_release(*mutex);
}

int hal_mutex_trylock(hal_mutex_t *mutex)
{
    if (!mutex)
    {
        return -1;
    }
    if (rt_mutex_take(*mutex, 0) == 0)
    {
        return 0;
    }
    return -2;
}

int hal_mutex_timedwait(hal_mutex_t *mutex, int ticks)
{
    if (!mutex)
    {
        return -1;
    }
    return rt_mutex_take(*mutex, ticks);
}
