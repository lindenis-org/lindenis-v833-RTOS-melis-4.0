/*
 * ===========================================================================================
 *
 *       Filename:  epos.c
 *
 *    Description:  Implementation of Interface needed by melis system API.
 *
 *        Version:  Melis3.0
 *         Create:  2018-03-12 11:57:39
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-04-29 17:45:32
 *
 * ===========================================================================================
 */

#include "epos.h"
#include <log.h>
#include <time.h>

#ifdef CONFIG_DEBUG_BACKTRACE
#include <backtrace.h>
#endif

static __cache_way_t LockICacheWays[MAX_ICACHE_LOCKED_WAY];
static __cache_way_t LockDCacheWays[MAX_DCACHE_LOCKED_WAY];
//static __u32 sram_stack[64];
//static __u32 old_stack;
//static __s32 LockResult;
static __u32 timer_error;

#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
#ifndef CONFIG_DYNAMIC_LOG_DEFAULT_LEVEL
#define CONFIG_DYNAMIC_LOG_DEFAULT_LEVEL 5
#endif
static volatile int g_log_level = CONFIG_DYNAMIC_LOG_DEFAULT_LEVEL;
#endif

void *__internal_malloc(__u32 size);
void __internal_free(void *ptr);

melis_malloc_context_t g_mem_leak_list =
{
    .list       = LIST_HEAD_INIT(g_mem_leak_list.list),
    .ref_cnt    = 0,
    .open_flag  = 0,
};

melis_thread_t melis_thread =
{
    .thr_id = {{0, RT_NULL}, }
};

#ifdef CONFIG_LOG_LEVEL_STORAGE_RTC
void hal_rtc_write_data(int index, __u32 val);
__u32 hal_rtc_read_data(int index);
#define RTC_LOG_LEVEL_INDEX 5

static void save_log_level_to_stroage(int level)
{
    __u32 rtc_log_level = 0;

    level += 1;
    rtc_log_level = hal_rtc_read_data(RTC_LOG_LEVEL_INDEX);
    rtc_log_level &= 0xfffffff0;
    rtc_log_level |= level & 0xf;
    hal_rtc_write_data(RTC_LOG_LEVEL_INDEX, rtc_log_level);
}

static int get_log_level_from_stroage(void)
{
    __u32 rtc_log_level = 0;

    rtc_log_level = hal_rtc_read_data(RTC_LOG_LEVEL_INDEX);
    if (rtc_log_level & 0xf)
    {
        g_log_level = (rtc_log_level & 0xf) - 1;
    }
    return g_log_level;
}
#elif CONFIG_LOG_LEVEL_STORAGE_NONE
static void save_log_level_to_stroage(int level)
{
    (void)(level);
}
static int get_log_level_from_stroage(void)
{
    return g_log_level;
}
#endif
/*
 * rtc register 5: 0-3bit for log level
 * rtc log level value : 0, rtc default value, no output log level
 * rtc log level value : 1, system log level is 0
 * rtc log level value : 2, system log level is 1
 */
#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
int get_log_level(void)
{
    int level;
    register rt_base_t temp;

    temp = rt_hw_interrupt_disable();

    level = get_log_level_from_stroage();

    rt_hw_interrupt_enable(temp);
    return level;
}

void set_log_level(int level)
{
    register rt_base_t temp;

    temp = rt_hw_interrupt_disable();

    save_log_level_to_stroage(level);
    g_log_level = level;

    rt_hw_interrupt_enable(temp);
}
#endif
void epos_memleak_detect_enable(void)
{
    register rt_base_t temp;

    temp = rt_hw_interrupt_disable();
    if (g_mem_leak_list.open_flag)
    {
        rt_hw_interrupt_enable(temp);
        return;
    }
    INIT_LIST_HEAD(&g_mem_leak_list.list);
    g_mem_leak_list.ref_cnt     = 1;
    g_mem_leak_list.open_flag   = 1;
    rt_hw_interrupt_enable(temp);

    return;
}

void epos_memleak_detect_disable(void)
{
    register rt_base_t      cpu_sr;
    struct list_head       *pos;
    struct list_head       *q;
    __u32 count = 0;
    __u32 i = 0;

    cpu_sr = rt_hw_interrupt_disable();

    __log("memory leak nodes:");
    list_for_each_safe(pos, q, &g_mem_leak_list.list)
    {
        melis_heap_buffer_node_t *tmp;
        tmp = list_entry(pos, melis_heap_buffer_node_t, i_list);
        rt_kprintf("\t %03d: ptr = 0x%08x, size = 0x%08x, entry = 0x%08x, thread = %s, tick = %d.\n", \
                   count ++, (__u32)tmp->vir, (__u32)tmp->size, (__u32)tmp->entry, tmp->name, tmp->tick);
#ifdef CONFIG_DEBUG_BACKTRACE
        for (i = 0; i < CONFIG_MEMORY_LEAK_BACKTRACE_LEVEL; i++)
        {
            if (tmp->caller[i] != NULL)
            {
                rt_kprintf("                backtrace : 0x%p\n", tmp->caller[i]);
            }
        }
#endif
        list_del(pos);
        rt_free(tmp);
    }
    g_mem_leak_list.ref_cnt   = 0;
    g_mem_leak_list.open_flag = 0;

    rt_hw_interrupt_enable(cpu_sr);
    return;
}

__s32 esMEMS_LockCache_Init(void)
{
    __s32 i;

    for (i = 0; i < MAX_ICACHE_LOCKED_WAY; i++)
    {
        LockICacheWays[i].addr    = (void *)(0xffffffff); /* invalid address   */
        LockICacheWays[i].lockbit = 0;                   /* invalid lockway bit*/
    }

    for (i = 0; i < MAX_DCACHE_LOCKED_WAY; i++)
    {
        LockDCacheWays[i].addr    = (void *)(0xffffffff); /* invalid address   */
        LockDCacheWays[i].lockbit = 0;                   /* invalid lockway bit*/
    }

    return 0;
}

void *rt_malloc(__u32 size)
{
    void *ptr;
    __u32 sp;

    melis_heap_buffer_node_t *new = NULL;

    if (rt_interrupt_get_nest() != 0)
    {
        __err("fatal error.");
        software_break();
    }

    ptr = __internal_malloc(size);
    if (ptr == RT_NULL)
    {
        return RT_NULL;
    }

    if (g_mem_leak_list.open_flag)
    {
        asm volatile("mov %0, r13\n":"=r"(sp));
        new = __internal_malloc(sizeof(melis_heap_buffer_node_t));
        if (new == RT_NULL)
        {
            goto RTN;
        }

        rt_enter_critical();
        rt_memset(new, 0x00, sizeof(melis_heap_buffer_node_t));

        INIT_LIST_HEAD(&new->i_list);

        new->size = size;
        new->vir = ptr;
        new->entry = (__u32)rt_thread_self()->entry;
        new->name = (__s8 *)rt_thread_self()->name;
        new->tick = rt_tick_get();
        new->stk = (void *)sp;

#ifdef CONFIG_DEBUG_BACKTRACE
        backtrace(NULL, new->caller, CONFIG_MEMORY_LEAK_BACKTRACE_LEVEL, 3, NULL);
#endif

        /*rt_memcpy(new->stack, (void*)sp, MEMLEAK_STK_DUMPSZ);*/
        list_add(&new->i_list, &g_mem_leak_list.list);

        rt_exit_critical();
    }
RTN:
    return ptr;
}

void rt_free(void *ptr)
{
    struct list_head       *pos;
    struct list_head       *q;
    melis_heap_buffer_node_t   *tmp;

    if (ptr == RT_NULL)
    {
        return;
    }

    if (rt_interrupt_get_nest() != 0)
    {
        __err("fatal error.");
        software_break();
    }

    __internal_free(ptr);

    if (g_mem_leak_list.open_flag)
    {
        rt_enter_critical();

        list_for_each_safe(pos, q, &g_mem_leak_list.list)
        {
            tmp = list_entry(pos, melis_heap_buffer_node_t, i_list);
            if (tmp->vir == ptr)
            {
                list_del(pos);
                rt_free(tmp);
                break;
            }
        }

        rt_exit_critical();
    }
    return;
}

void *esMEMS_Balloc(__u32 size)
{
    return rt_malloc(size);
}

void esMEMS_Bfree(void *addr, __u32 size)
{
    rt_free(addr);
}

__s32 esMEMS_Info(void)
{
    rt_uint32_t total, used, max_used;

    rt_memory_info(&total, &used, &max_used);

    __log("Total heap size 0x%08x, used 0x%08x, max_used 0x%08x, aval margin 0x%08x.", \
          total, used, max_used, total - max_used);

    return 0;
}

void esMEMS_Mfree(void *heap, void *ptr)
{
    rt_free(ptr);
    return;
}

void *esMEMS_Malloc(void *heap, __u32 size)
{
    return rt_malloc(size);
}

__u32 esMEMS_VA2PA(__u32 vaddr)
{
    return awos_arch_vaddr_map2_phys(vaddr);
}

__u32 esKRNL_TCreate(void (*entry)(void *p_arg), void *p_arg, __u32 stksize, __u16 id_priolevel)
{
    __u8   prio;
    __u8   priolevel = id_priolevel & 0xff;
    __u32  slice = 20;
    __s32  i;
    char   name[RT_NAME_MAX] = {0};

    if ((__u32)entry < (1 << FCSEID_SHIFT))
    {
        __err("fatal error, cant not support fcse function!");
        software_break();
        return 0;
    }

    rt_enter_critical();

    for (i = 0; i < MELIS_THREAD_COUNT; i ++)
    {
        if (melis_thread.thr_id[i].rt_thread_id == RT_NULL)
        {
            break;
        }
    }

    rt_exit_critical();

    if (i >= MELIS_THREAD_COUNT)
    {
        __err("error when creating new melis thread.");
        software_break();
        return 0;
    }

    if ((priolevel == KRNL_priolevel0) || \
        (priolevel == KRNL_priolevel1) || \
        (priolevel == KRNL_priolevel2))
    {
        //audio feed PCM task should be in high priority.
        //drv_audio 'audio_play_main_task'
        prio = 9;
    }
    else if (priolevel == 0x38)
    {
        prio = 10;
    }
    else if (priolevel == 0x39)
    {
        prio = 11;
    }
    else if (priolevel == 0x3b)
    {
        //let vdrv_maintask thread as quick as possiable.
        //prio = 12;
        prio = 2;
    }
    else if (priolevel == 0x58)
    {
        prio = 13;
    }
    else if (priolevel == 0x59)
    {
        prio = 14;
    }
    else if (priolevel == 0x78)
    {
        prio = 16;
    }
    else if (priolevel == 0x79)
    {
        prio = 17;
    }
    else if (priolevel == 0x7A)
    {
        prio = 18;
    }
    else if (priolevel == 0x7B)
    {
        prio = 19;
    }
    else if (priolevel == 0x7C)
    {
        prio = 20;
    }
    else if (priolevel == 0x7D)
    {
        prio = 21;
    }
    else if (priolevel == 0x7E)
    {
        prio = 22;
    }
    else if (priolevel == 0x7F)
    {
        prio = 23;
    }
    else
    {
        prio = 24;
        slice = 15;
    }

    rt_snprintf(name, sizeof(name), MELIS_TASK_PREFIX"%03d", i + 1);
    melis_thread.thr_id[i].rt_thread_id = rt_thread_create(name, entry, p_arg, stksize, prio, slice);
    if (melis_thread.thr_id[i].rt_thread_id == NULL)
    {
        __err("failure to create melis thread.!");
        software_break();
        return 0;
    }

    melis_thread.thr_id[i].running = 1;
    rt_thread_startup(melis_thread.thr_id[i].rt_thread_id);

#ifdef USE_MELIS_SHORT_THREAD_ID
    return i + 1;
#else
    return (__u32)melis_thread.thr_id[i].rt_thread_id;
#endif
}

/* must be protected by scheduler lock */
static __s32 find_thread(__u32 prio)
{
    __s32 i, idx = 0;
    __u8 found;

    found = 0;
    if (prio == EXEC_prioself)
    {
        for (i = 0; i < MELIS_THREAD_COUNT; i ++)
        {
            if (melis_thread.thr_id[i].rt_thread_id == rt_thread_self())
            {
                found = 1;
                break;
            }
        }

        if ((i == MELIS_THREAD_COUNT) || !found)
        {
            __err("fatal error, cant found thread self. i: %d, found: %d.", i, found);
            software_break();
            return -1;
        }

        idx = i + 1;
    }
    else
    {
#ifdef USE_MELIS_SHORT_THREAD_ID
        if (prio > MELIS_THREAD_COUNT)
        {
            __err("fatal error, melis thread id: %d, prio: %d.", idx, prio);
            software_break();
            return -1;
        }
        idx =  prio & 0xff;
#else
        for (i = 0; i < MELIS_THREAD_COUNT; i ++)
        {
            if ((__u32)melis_thread.thr_id[i].rt_thread_id == prio)
            {
                found = 1;
                break;
            }
        }

        if ((i == MELIS_THREAD_COUNT) || !found)
        {
            __err("fatal error, cant found thread self. i: %d, found: %d.", i, found);
            return -1;
        }

        idx = i + 1;
#endif
    }

    if ((idx > MELIS_THREAD_COUNT) || (idx <= 0))
    {
        __err("fatal error, melis thread id: %d, prio: %d.", idx, prio);
        software_break();
        return -1;
    }

    return idx;
}

static rt_int8_t __task_del(__u32 prio)
{
    __s32 idx = 0;
    rt_thread_t tmp;

    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (!melis_thread.thr_id[idx - 1].rt_thread_id)
    {
        rt_exit_critical();
        __err("fatal error, cant found right thread handle.");
        return OS_TASK_NOT_EXIST;
    }

    __msg("thread %d(entry: 0x%p) being deleted!", idx, melis_thread.thr_id[idx - 1].rt_thread_id->entry);

    tmp = melis_thread.thr_id[idx - 1].rt_thread_id;
    melis_thread.thr_id[idx - 1].rt_thread_id = RT_NULL;
    melis_thread.thr_id[idx - 1].running = 0;

    if (prio == EXEC_prioself)
    {
        void rt_thread_exit(void);
        rt_exit_critical();
        rt_thread_exit();
        CODE_UNREACHABLE;
    }
    else
    {
        rt_thread_delete(tmp);
        rt_exit_critical();
    }

    return 0;
}

static rt_int8_t __thread_del_req(__u32 prio)
{
    __s32 idx;

#ifdef USE_MELIS_SHORT_THREAD_ID
    if ((prio > MELIS_THREAD_COUNT) || (prio <= 0))
    {
        __err("fatal error,  prio: %d.", prio);
        software_break();
        return OS_PRIO_INVALID;
    }
#endif

    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (melis_thread.thr_id[idx - 1].rt_thread_id == RT_NULL)
    {
        rt_exit_critical();
        __msg("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (prio == EXEC_prioself)
    {
        rt_exit_critical();
        return melis_thread.thr_id[idx - 1].running;
    }
    else
    {
        melis_thread.thr_id[idx - 1].running = OS_TASK_DEL_REQ;
        rt_exit_critical();
        return OS_NO_ERR;
    }
}

//just return to rt_thread_exit from user entry.
rt_int8_t esKRNL_TDel(__u32 prio)
{
    return __task_del(prio);
}

rt_int8_t esKRNL_TaskDel(__u32 prio)
{
    return __task_del(prio);
}

rt_int8_t esKRNL_TDelReq(__u32 prio_ex)
{
#ifdef USE_MELIS_SHORT_THREAD_ID
    prio_ex &= 0xff;
#endif
    return __thread_del_req(prio_ex);
}

rt_int8_t esKRNL_TaskDelReq(__u32 prio_ex)
{
    return __thread_del_req(prio_ex);
}

__u8 esKRNL_TaskQuery(__u32 prio, __krnl_tcb_t *p_task_data)
{
    __s32 idx;

    if (p_task_data == NULL)
    {
        return OS_ERR_PDATA_NULL;
    }
#ifdef USE_MELIS_SHORT_THREAD_ID

    if (prio >= 0xff)
    {
        if (prio != OS_PRIO_SELF)
        {
            __err("error, invalid prio");
            software_break();
            return (OS_PRIO_INVALID);
        }
    }

    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }
    rt_exit_critical();

    p_task_data->OSTCBPrio = idx;
#else
    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread(entry 0x%p) has been deleted!", ((rt_thread_t)prio)->entry);
        return OS_TASK_NOT_EXIST;
    }
    rt_exit_critical();

    p_task_data->OSTCBPrio = (__u32)melis_thread.thr_id[idx - 1].rt_thread_id;
#endif

    return OS_NO_ERR;
}

__pCBK_t esKRNL_GetCallBack(__pCBK_t cb)
{
    __u32 fcseid = 0;

    if (((__u32)cb >> FCSEID_SHIFT) != 0)
    {
        return cb;
    }

    asm volatile("mrc p15, 0, %0, c13, c0, 0\t\n":"=r"(fcseid));

    return (__pCBK_t)(fcseid | (__u32)cb);
}

__s32 esKRNL_CallBack(__pCBK_t cb, void *arg)
{
    register rt_base_t cpu_sr;
    __u32 fcseid, fcse;
    __s32 ret;

    if ((__u32)cb >= NOFCSE_AREA)
    {
        return cb(arg);
    }

    fcse = (__u32)cb >> FCSEID_SHIFT;
    fcse <<= FCSEID_SHIFT;

    cpu_sr = rt_hw_interrupt_disable();
    asm volatile("mrc p15, 0, %0, c13, c0, 0\t\n":"=r"(fcseid));
    asm volatile("mcr p15, 0, %0, c13, c0, 0\t\n" :: "r"(fcse));
    rt_hw_interrupt_enable(cpu_sr);

    ret = ((__pCBK_t)(((__u32)cb) & ((1 << FCSEID_SHIFT) - 1)))(arg);

    asm volatile("mcr p15, 0, %0, c13, c0, 0\t\n" :: "r"(fcseid));

    return ret;
}

__u8 esKRNL_TimeDlyResume(__u32 prio)
{
    register rt_base_t temp;
    __s32 idx;

#ifdef USE_MELIS_SHORT_THREAD_ID
    if (prio > MELIS_THREAD_COUNT)
    {
        __err("fatal error, melis thread prio: %d.", prio);
        software_break();
        return -1;
    }
    prio &= 0xff;
#endif
    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("cant find specify thread:0x%08x.", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (melis_thread.thr_id[idx - 1].rt_thread_id == RT_NULL)
    {
        rt_exit_critical();
        __err("cant find specify thread:0x%08x.", prio);
        return OS_TASK_NOT_EXIST;
    }
    rt_exit_critical();

    temp = rt_hw_interrupt_disable();
    if (melis_thread.thr_id[idx - 1].rt_thread_id->stat != RT_THREAD_SUSPEND)
    {
        rt_hw_interrupt_enable(temp);
        return OS_TASK_NOT_EXIST;
    }

    melis_thread.thr_id[idx - 1].rt_thread_id->error = -RT_EINVAL;
    rt_thread_resume(melis_thread.thr_id[idx - 1].rt_thread_id);
    rt_hw_interrupt_enable(temp);

    return OS_NO_ERR;
}

__hdle esKRNL_SemCreate(__u16 count)
{
    if (rt_interrupt_get_nest() != 0)
    {
        __err("create sem in isr.");
        return NULL;
    }

    return (__hdle)rt_sem_create("melis_sem", count, 0);
}

__hdle esKRNL_SemDel(__hdle sem, __u8 opt, __u8 *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_DEL_ISR;
        }
        __err("delete sem in isr.");
        return sem;
    }
    if (sem == RT_NULL)
    {
        if (err)
        {
            *err =  OS_ERR_PEVENT_NULL;
        }
        return sem;
    }

    rt_sem_delete(sem);

    if (err)
    {
        *err = OS_NO_ERR;
    }

    return RT_NULL;
}

void esKRNL_SemPend(__hdle sem, __u16 timeout, __u8 *err)
{
    rt_err_t ret;

    if (sem == RT_NULL)
    {
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        /*__err("invalid parameter.");*/
        return;
    }

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend sem in isr.");
        return;
    }

    if (rt_critical_level() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_LOCKED;
        }
        __err("pend sem in sched lock.");
        return;
    }

    if (timeout == 0)
    {
        ret = rt_sem_take(sem, RT_WAITING_FOREVER);
    }
    else
    {
        ret = rt_sem_take(sem, timeout);
    }

    if (err)
    {
        if (ret != RT_EOK)
        {
            *err = OS_TIMEOUT;
        }
        else
        {
            *err = OS_NO_ERR;
        }
    }

    return;
}

__u8 esKRNL_SemPost(__hdle sem)
{
    rt_err_t ret;

    if (sem == RT_NULL)
    {
        return OS_ERR_PEVENT_NULL;
    }

    ret = rt_sem_release(sem);
    if (ret != RT_EOK)
    {
        return OS_SEM_OVF;
    }

    return OS_NO_ERR;
}

//TODO, need investigation
__u8 esKRNL_SemQuery(__hdle sem, OS_SEM_DATA *p_sem_data)
{
    __u16 value;

    if (sem == NULL)
    {
        __err("invalid parameter.");
        return OS_ERR_PEVENT_NULL;
    }

    if (p_sem_data == NULL)
    {
        __err("invalid parameter.");
        return OS_ERR_PDATA_NULL;
    }

    rt_memset(p_sem_data, 0x00, sizeof(OS_SEM_DATA));
    rt_sem_control(sem, RT_IPC_CMD_GET_STATE, &value);

    p_sem_data->OSCnt = value;
    return OS_NO_ERR;
}

void esKRNL_SemSet(__hdle sem, __u16 cnt, __u8 *err)
{
    rt_uint32_t value = cnt;

    if (sem == NULL)
    {
        __err("invalid parameter.");
        return;
    }

    rt_sem_control(sem, RT_IPC_CMD_RESET, (void *)value);

    if (err)
    {
        *err = OS_NO_ERR;
    }
    return;
}

__hdle esKRNL_QCreate(__u16 size)
{
    if (rt_interrupt_get_nest() != 0)
    {
        __err("create msgq in isr.");
        return NULL;
    }
    return (__hdle)rt_mb_create("melis_app", size, 0);
}

__hdle esKRNL_QDel(__hdle pevent, __u8 opt, __u8 *err)
{
    if (pevent == NULL)
    {
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        __err("invalid parameter.");
        return NULL;
    }

    if (rt_interrupt_get_nest() != 0)
    {
        __err("delete msgq in isr.");
        return pevent;
    }
    rt_mb_delete(pevent);
    if (err)
    {
        *err = OS_NO_ERR;
    }

    return NULL;
}

__u32 esKRNL_QAccept(__hdle pevent, __u8 *err)
{
    __u32 value;
    rt_err_t ret;

    if (pevent == NULL)
    {
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        __err("invalid parameter.");
        return 0;
    }

    ret = rt_mb_recv(pevent, &value, 0);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = KRNL_Q_EMPTY;
        }
        return 0;
    }
    else
    {
        if (err)
        {
            *err = OS_NO_ERR;
        }
    }

    return value;
}
__u32 esKRNL_QPend(__hdle pevent, __u16 timeout, __u8 *err)
{
    __u32 value;
    __s32 tmo;
    rt_err_t ret;

    if (err)
    {
        *err = OS_NO_ERR;
    }

    if (pevent == NULL)
    {
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        __err("invalid parameter.");
        return 0;
    }
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend msgq in isr.");
        return 0;
    }

    if (rt_critical_level() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_LOCKED;
        }
        __err("pend msgq in sched lock.");
        return 0;
    }

    if (timeout == 0)
    {
        tmo = -1;
    }
    else
    {
        tmo = timeout;
    }

    ret = rt_mb_recv(pevent, &value, tmo);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = OS_TIMEOUT;
        }
        return 0;
    }

    return value;
}

__u8 esKRNL_QPost(__hdle pevent, __u32 msg)
{
    rt_err_t ret;

    if (pevent == NULL)
    {
        __err("invalid parameter.");
        return OS_ERR_PEVENT_NULL;
    }

    ret = rt_mb_send(pevent, msg);
    if (ret != RT_EOK)
    {
        if (ret == RT_EFULL)
        {
            return KRNL_Q_FULL;
        }
        else
        {
            return OS_TIMEOUT;
        }
    }

    return OS_NO_ERR;
}

__u8 esKRNL_QFlush(__hdle pevent)
{
    if (pevent == NULL)
    {
        __err("invalid parameter.");
        return OS_ERR_PEVENT_NULL;
    }

    rt_mb_control(pevent, RT_IPC_CMD_RESET, RT_NULL);

    return OS_NO_ERR;
}

__u8 esKRNL_QQuery(__hdle pevent, __krnl_q_data_t *p_q_data)
{
    __u32 entry;

    if (!pevent || !p_q_data)
    {
        __err("invalid parameter.");
        return OS_ERR_PEVENT_NULL;
    }

    rt_mb_control(pevent, RT_IPC_CMD_GET_STATE, &entry);
    p_q_data->OSNMsgs = entry;

    return OS_NO_ERR;
}

__hdle esKRNL_MboxCreate(__u32 msg)
{
    __hdle mb;

    if (rt_interrupt_get_nest() != 0)
    {
        __err("create mailbox in isr.");
        return NULL;
    }

    mb = (__hdle)rt_mb_create("melis_app", 1, 0);
    if (mb != NULL)
    {
        rt_mb_send(mb, msg);
    }

    return mb;
}

__u32 esKRNL_MboxAccept(__hdle pevent)
{
    __u32 value;
    rt_err_t  err;

    err = rt_mb_recv(pevent, &value, 0);
    if (err != RT_EOK)
    {
        return 0;
    }

    return value;
}

__u32 esKRNL_MboxDel(__hdle pevent, __u8 opt, __u8 *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_DEL_ISR;
        }
        __err("delete mailbox in isr.");
        return (__u32)pevent;
    }

    rt_mb_delete(pevent);
    if (err)
    {
        *err = OS_NO_ERR;
    }

    return 0;
}

__u32 esKRNL_MboxPend(__hdle pevent, __u16 timeout, __u8 *err)
{
    rt_ubase_t  value;
    rt_err_t    ret;
    rt_int32_t  tmo;

    if (err)
    {
        *err = OS_NO_ERR;
    }

    if (pevent == NULL)
    {
        __err("invalid parameter.");
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        return 0;
    }

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend mailbox in isr.");
        return 0;
    }

    if (rt_critical_level() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_LOCKED;
        }
        __err("pend msgq in sched lock.");
        return 0;
    }

    if (timeout == 0)
    {
        tmo = -1;
    }
    else
    {
        tmo = timeout;
    }

    ret = rt_mb_recv(pevent, &value, tmo);
    if (ret != RT_EOK)
    {
        *err = OS_TIMEOUT;
        return 0;
    }

    if (err)
    {
        *err = OS_NO_ERR;
    }
    return value;
}

__u8 esKRNL_MboxPost(__hdle pevent, __u32 msg)
{
    rt_err_t  ret;

    if (pevent == NULL)
    {
        __err("invalid paramter.");
        return OS_ERR_PEVENT_NULL;
    }

    ret = rt_mb_send(pevent, msg);
    if (ret != RT_EOK)
    {
        __err("post paramter error.");
        return OS_MBOX_FULL;
    }

    return OS_NO_ERR;
}

__hdle esKRNL_FlagCreate(__u32 init_value, __u8 *err)
{
    __hdle event;

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_CREATE_ISR;
        }
        __err("create flag in isr.");
        return NULL;
    }

    if (err)
    {
        *err = OS_NO_ERR;
    }

    event = (__hdle)rt_event_create("melis_event", init_value, RT_IPC_FLAG_FIFO);
    if (event == NULL)
    {
        if (err)
        {
            *err = KRNL_FLAG_GRP_DEPLETED;
        }
    }

    return (__hdle)event;
}

__u32 esKRNL_FlagAccept(__hdle pgrp, __u32 flags, __u8 wait_type, __u8 *err)
{
    rt_err_t    ret;
    rt_uint32_t recved;
    __u8 result, options;

    if (err)
    {
        *err = OS_NO_ERR;
    }

    options = 0;
    result  = (__u8)(wait_type & OS_FLAG_CONSUME);

    if (result != 0)
    {
        wait_type &= ~OS_FLAG_CONSUME;
        options   |= RT_EVENT_FLAG_CLEAR;
    }

    if (wait_type == OS_FLAG_WAIT_SET_ALL)
    {
        options |= RT_EVENT_FLAG_AND;
    }
    else if (wait_type == OS_FLAG_WAIT_SET_ANY)
    {
        options |= RT_EVENT_FLAG_OR;
    }
    else if (wait_type == OS_FLAG_WAIT_CLR_ALL)
    {
        __err("error flag type OS_FLAG_WAIT_CLR_ALL");
        software_break();
    }
    else if (wait_type == OS_FLAG_WAIT_CLR_ANY)
    {
        __err("error flag type OS_FLAG_WAIT_CLR_ANY");
        software_break();
    }
    else
    {
        __err("error flag type");
        software_break();
    }

    ret = rt_event_recv(pgrp, flags, options, 0, &recved);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = OS_FLAG_ERR_NOT_RDY;
        }
        return 0;
    }

    return recved;
}

__u32 esKRNL_FlagDel(__hdle pgrp, __u8 opt, __u8 *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_DEL_ISR;
        }
        __err("delete flag in isr.");
        return (__u32)pgrp;
    }

    rt_event_delete(pgrp);
    if (err)
    {
        *err = OS_NO_ERR;
    }

    return 0;
}

__u32 esKRNL_FlagPend(__hdle pgrp, __u32 flags, __u32 waittype_timeout, __u8 *err)
{
    rt_uint32_t recved;
    __s32 tmo;
    __u16 timeout = waittype_timeout & 0xffff;
    __u8 wait_type = waittype_timeout >> 16;
    __u8 result, options;
    rt_err_t ret;

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend flag in isr.");
        return 0;
    }

    if (err)
    {
        *err = OS_NO_ERR;
    }

    if (timeout == 0)
    {
        tmo = RT_WAITING_FOREVER;
    }
    else
    {
        tmo = timeout;
    }

    options = 0;
    result = (__u8)(wait_type & OS_FLAG_CONSUME);
    if (result != 0)
    {
        wait_type &= ~OS_FLAG_CONSUME;
        options   |= RT_EVENT_FLAG_CLEAR;
    }

    if (wait_type == OS_FLAG_WAIT_SET_ALL)
    {
        options |= RT_EVENT_FLAG_AND;
    }
    else if (wait_type == OS_FLAG_WAIT_SET_ANY)
    {
        options |= RT_EVENT_FLAG_OR;
    }
    else if (wait_type == OS_FLAG_WAIT_CLR_ALL)
    {
        __err("not support flag type OS_FLAG_WAIT_CLR_ALL!");
        software_break();
    }
    else if (wait_type == OS_FLAG_WAIT_CLR_ANY)
    {
        __err("not support flag type OS_FLAG_WAIT_CLR_ANY!");
        software_break();
    }
    else
    {
        __err("not support flag type!");
        software_break();
    }

    ret = rt_event_recv(pgrp, flags, options, tmo, &recved);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = OS_FLAG_ERR_NOT_RDY;
        }
        return 0;
    }

    return recved;
}

__u32 esKRNL_FlagPost(__hdle pgrp, __u32 flags, __u8 opt, __u8 *err)
{
    __u32 value;

    if (opt == OS_FLAG_CLR)
    {
        __err("not support flag type!");
        software_break();
        return OS_FLAG_INVALID_OPT;
    }

    rt_event_send(pgrp, flags);
    rt_event_control(pgrp, RT_IPC_CMD_GET_STATE, &value);

    if (err)
    {
        *err = OS_NO_ERR;
    }

    return value;
}

__u32 esKRNL_FlagQuery(__hdle pgrp, __u8 *err)
{
    __u32 value;

    rt_event_control(pgrp, RT_IPC_CMD_GET_STATE, &value);

    if (err)
    {
        *err = OS_NO_ERR;
    }

    return value;
}

__hdle esKRNL_TmrCreate(__u32 period, __u8 opt, OS_TMR_CALLBACK callback, void *callback_arg)
{
    __u8 flag = 0;
    char name[16] = { '\0', };

    if (period == 0)
    {
        return RT_NULL;
    }

    period = (period < 10) ? 1 : (period / 10);
    if ((opt & OS_TMR_OPT_PRIO_MASK) == OS_TMR_OPT_PRIO_LOW)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("create timer in isr.");
            return NULL;
        }
        flag |= RT_TIMER_FLAG_SOFT_TIMER;
    }

    if ((opt & OS_TMR_OPT_NORMAL_MASK) == OS_TMR_OPT_PERIODIC)
    {
        flag |= RT_TIMER_FLAG_PERIODIC;
    }

    rt_memset(name, 0, 16);
    rt_snprintf(name, sizeof(name), "os-tmr-""%04d", period);
    return (__hdle)rt_timer_create(name, callback, callback_arg, period, flag);
}

__s32 esKRNL_TmrDel(__hdle ptmr)
{
    rt_err_t ret;
    rt_uint8_t flag = 0;

    if (ptmr == NULL)
    {
        __err("invalid parameter.");
        software_break();
        return 0;
    }

    rt_timer_control(ptmr, RT_TIMER_CTRL_GET_STATE, &flag);
    if (flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("delete timer in isr.");
            return 0;
        }
    }

    ret = rt_timer_delete((rt_timer_t)ptmr);

    return (ret == RT_EOK) ? 1 : 0;
}

__s32 esKRNL_TmrStart(__hdle ptmr)
{
    rt_err_t err;
    __u8 flag = 0;

    if (ptmr == NULL)
    {
        __err("invalid parameter.");
        software_break();
        return 0;
    }

    rt_timer_control(ptmr, RT_TIMER_CTRL_GET_STATE, &flag);
    if (flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("start timer in isr.");
            return 0;
        }
    }

    err = rt_timer_start((rt_timer_t)ptmr);

    return (err == RT_EOK) ?  1 : 0;
}

__s32 esKRNL_TmrStop(__hdle ptmr, rt_int8_t opt, void *callback_arg)
{
    __u8 flag = 0;
    rt_err_t ret;

    if (ptmr == NULL)
    {
        __err("paramer invalid.");
        software_break();
        return 0;
    }

    rt_timer_control(ptmr, RT_TIMER_CTRL_GET_STATE, &flag);
    if (flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("stop timer in isr.");
            return 0;
        }
    }

    ret = rt_timer_stop((rt_timer_t)ptmr);
    return (ret == RT_EOK) ?  EPDK_TRUE : EPDK_FAIL;
}

__u8 esKRNL_TmrStateGet(__hdle ptmr)
{
    __u8 flag = 0;

    if (ptmr == NULL)
    {
        __err("invalid parameter.");
        return 0;
    }
    rt_timer_control(ptmr, RT_TIMER_CTRL_GET_STATE, &flag);

    if (flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("delete timer in isr.");
            return 0;
        }
    }

    if (flag & RT_TIMER_FLAG_ACTIVATED)
    {
        return OS_TMR_STATE_RUNNING;
    }
    else
    {
        return OS_TMR_STATE_STOPPED;
    }
}

__s16 esKRNL_TmrError(__hdle ptmr)
{
    if (ptmr == NULL)
    {
        __err("invalid parameter.");
        software_break();
        return timer_error;
    }

    return 0;
}

__hdle esKRNL_SktCreate(__u32 depth, __u32 dbuf_attr, __u32 mbuf_attr)
{
    __u32  i, dbufsize, mbufsize, mallocsize;
    void   *dbufarray;
    void   *mbufarray;
    void   *tmpptr;

    __krnl_skt_t    *pSKT;
    __krnl_sktfrm_t *frmbufarray;

    dbufsize = (dbuf_attr & ~KRNL_SKT_BUF_TYP_MASK);
    mbufsize = (mbuf_attr & ~KRNL_SKT_BUF_TYP_MASK);

    //calculate the memory size for create the socket
    mallocsize = sizeof(__krnl_skt_t) +                /* socket handle area                                           */
                 sizeof(__krnl_sktfrm_t) * depth;      /* socket frame message area                                    */

    //check data buffer type and calculate data buffer size
    if (dbufsize)
    {
        if ((dbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_UNSEQ)
        {
            mallocsize += dbufsize * depth;
        }
    }

    //check message buffer type and calculate message buffer size
    if (mbufsize)
    {
        if ((mbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_UNSEQ)
        {
            mallocsize += mbufsize * depth;
        }
    }

    pSKT = (__krnl_skt_t *)rt_malloc(mallocsize);
    if (pSKT == RT_NULL)
    {
        __err("Malloc memory failed!");
        goto _err0_createsocket;
    }
    rt_memset(pSKT, 0, mallocsize);

    //process the address of frame
    frmbufarray = (__krnl_sktfrm_t *)((__u32)pSKT +  sizeof(__krnl_skt_t));
    tmpptr = (void *)((__u32)frmbufarray + sizeof(__krnl_sktfrm_t) * depth);

    //process the address for data buffer
    if (dbufsize)
    {
        if ((dbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_UNSEQ)
        {
            dbufarray = tmpptr;
            tmpptr = (void *)((__u32)dbufarray + dbufsize * depth);
        }
        else
        {
            dbufarray = rt_malloc(dbufsize * depth);
            if (dbufarray == (void *)0)
            {
                __err("alloc memory failed!");
                goto _err1_createsocket;
            }
            rt_memset(dbufarray, 0, dbufsize * depth);
        }
    }
    else
    {
        dbufarray = RT_NULL;
    }

    //process the address for message buffer
    if (mbufsize)
    {
        if ((mbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_UNSEQ)
        {
            mbufarray = tmpptr;
        }
        else
        {
            mbufarray = rt_malloc(mbufsize * depth);
            if (mbufarray == (void *)0)
            {
                __err("Balloc memory failed!");
                goto _err2_createsocket;
            }
            rt_memset(mbufarray, 0, mbufsize * depth);
        }
    }
    else
    {
        mbufarray = (void *)0;
    }

    //create input queue and output queue for socket
    pSKT->mb_in = rt_mb_create("socket-i", depth, 0);
    if (pSKT->mb_in == RT_NULL)
    {
        __err("Input queue create failed!");
        goto _err3_CreateSocket;
    }
    pSKT->mb_out = rt_mb_create("socket-o", depth, 0);
    if (pSKT->mb_out == RT_NULL)
    {
        __err("Out queue create failed!");
        goto _err4_CreateSocket;
    }

    pSKT->depth = depth;                            /* the depth of the input and out Q                             */
    pSKT->bufsize = dbufsize;                       /* data buffer size                                             */
    pSKT->msgsize = mbufsize;                       /* message buffer size                                          */
    pSKT->dbufmod = dbuf_attr & KRNL_SKT_BUF_TYP_MASK; /* data buffer type                                          */
    pSKT->mbufmod = mbuf_attr & KRNL_SKT_BUF_TYP_MASK; /* message buffer type                                       */
    pSKT->dbufaddr = dbufarray;                     /* pointer to data buffer start addr                            */
    pSKT->mbufaddr = mbufarray;                     /* pointer to message buffer start addr                         */

    //put the message frame into the input queue
    for (i = 0; i < depth; i ++)
    {
        frmbufarray[i].size = sizeof(__krnl_sktfrm_t);

        if (dbufsize)
        {
            frmbufarray[i].data = (void *)((__u32)dbufarray + i * dbufsize);
            frmbufarray[i].datsize = dbufsize;
        }

        if (mbufsize)
        {
            frmbufarray[i].msg = (void *)((__u32)mbufarray + i * mbufsize);
            frmbufarray[i].msgsize = mbufsize;
        }

        if (rt_mb_send(pSKT->mb_in, (__u32)&frmbufarray[i]) != RT_EOK)
        {
            __err("fatal error, socket inbox not filled.");
            software_break();
        }
    }

    return (__hdle)pSKT;

_err4_CreateSocket:
    rt_mb_delete(pSKT->mb_in);
_err3_CreateSocket:
    if (mbufsize && ((mbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_SEQ))
    {
        rt_free(mbufarray);
    }
_err2_createsocket:
    if (dbufsize && ((dbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_SEQ))
    {
        rt_free(dbufarray);
    }
_err1_createsocket:
    rt_free(pSKT);
_err0_createsocket:
    return NULL;
}

__s32 esKRNL_SktDel(__hdle skt, __u8 opt)
{
    rt_err_t ret;
    __krnl_skt_t *tmp_skt = (__krnl_skt_t *)skt;

    if (!tmp_skt || !tmp_skt->mb_out || !tmp_skt->mb_in)
    {
        __err("error of socket.");
        software_break();
        return -1;
    }

    ret = rt_mb_delete(tmp_skt->mb_in);
    if (ret != RT_EOK)
    {
        return -1;
    }

    ret = rt_mb_delete(tmp_skt->mb_out);
    if (ret != RT_EOK)
    {
        return -1;
    }

    //release data buffer
    if ((tmp_skt->dbufmod & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_SEQ)
    {
        rt_free(tmp_skt->dbufaddr);
        tmp_skt->dbufaddr = NULL;
    }

    //release message buffer
    if ((tmp_skt->mbufmod & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_SEQ)
    {
        rt_free(tmp_skt->mbufaddr);
        tmp_skt->mbufaddr = NULL;
    }

    rt_free(tmp_skt);
    return 0;
}

__krnl_sktfrm_t *esKRNL_SktPend(__hdle skt, __u8 user, __u32 timeout)
{
    __krnl_skt_t *tmp_skt = (__krnl_skt_t *)skt;
    rt_err_t      err = RT_ERROR;
    __s32         tmo;
    __u32         value;

    if (!tmp_skt || !tmp_skt->mb_out || !tmp_skt->mb_in)
    {
        __err("error of socket.");
        software_break();
        return NULL;
    }

    if (timeout == 0)
    {
        tmo = RT_WAITING_FOREVER;
    }
    else
    {
        tmo = timeout;
    }

    if (user == KRNL_SKT_USR_IN)
    {
        //if the user is data output type, request the input queue, get message frame
        err = rt_mb_recv(tmp_skt->mb_in, &value, tmo);
    }
    else if (user == KRNL_SKT_USR_OUT)
    {
        //if the user is data input type, request the output queue, get message frame
        err = rt_mb_recv(tmp_skt->mb_out, &value, tmo);
    }
    else
    {
        __err("fatal error, not support skt method.");
        software_break();
    }

    if (err == RT_EOK)
    {
        return (__krnl_sktfrm_t *)value;
    }

    return NULL;
}

__s32 esKRNL_SktPost(__hdle skt, __u8 user, __krnl_sktfrm_t *frm)
{
    __krnl_skt_t        *tmp_skt = (__krnl_skt_t *)skt;

    if (!tmp_skt || !tmp_skt->mb_out || !tmp_skt->mb_in)
    {
        __err("error of socket.");
        software_break();
        return -1;
    }

    if (user == KRNL_SKT_USR_IN)
    {
        //if the user is data output type, post the data buffer to other modules
        return rt_mb_send(tmp_skt->mb_out, (__u32)frm);
    }
    else if (user == KRNL_SKT_USR_OUT)
    {
        //if the user is data input type, post the free buffer to other modules
        return rt_mb_send(tmp_skt->mb_in, (__u32)frm);
    }
    else
    {
        __err("fatal error, not support skt method.");
        software_break();
    }

    return -1;
}

__krnl_sktfrm_t *esKRNL_SktAccept(__hdle skt, __u8 user)
{
    __u32               value;
    rt_err_t            err = RT_ERROR;
    __krnl_sktfrm_t    *ret;
    __krnl_skt_t        *tmp_skt = (__krnl_skt_t *)skt;

    if (!tmp_skt || !tmp_skt->mb_out || !tmp_skt->mb_in)
    {
        __err("error of socket.");
        software_break();
        return NULL;
    }

    if (user == KRNL_SKT_USR_IN)
    {
        //if the user is data output type, request the input queue, get message frame
        err = rt_mb_recv(tmp_skt->mb_in, &value, 0);
    }
    else if (user == KRNL_SKT_USR_OUT)
    {
        //if the user is data input type, request the output queue, get message frame
        err = rt_mb_recv(tmp_skt->mb_out, &value, 0);
    }
    else
    {
        __err("fatal error, not support skt method.");
        software_break();
    }

    if (err == RT_EOK)
    {
        tmp_skt->err = OS_NO_ERR;
        ret = (__krnl_sktfrm_t *)value;
    }
    else
    {
        tmp_skt->err = -1;
        ret = NULL;
    }

    return ret;
}

__u16 esKRNL_Version(void)
{
#define KRNL_VERSION            313u
    return KRNL_VERSION;
}

void esKRNL_TimeDly(__u16 ticks)
{
    rt_thread_delay(ticks);
}

void esKRNL_SchedLock(void)
{
    rt_enter_critical();
}

void esKRNL_SchedUnlock(void)
{
    rt_exit_critical();
}

unsigned long esKRNL_InterruptDisable(void)
{
    return (unsigned long)rt_hw_interrupt_disable();
}

void esKRNL_InterruptEnable(unsigned long level)
{
    rt_hw_interrupt_enable(level);
}

__u32 esKRNL_TimeGet(void)
{
    return rt_tick_get();
}

void esKRNL_TimeSet(__u32 ticks)
{
    rt_tick_set(ticks);
}

__u32 esKRNL_Time(void)
{
    return rt_tick_get();
}

__u16 esKRNL_SemAccept(void *psem)
{
    rt_err_t ret;
    rt_sem_t  sem = (rt_sem_t)psem;

    if (sem == RT_NULL)
    {
        return 0;
    }

    ret = rt_sem_trytake(sem);
    if (ret == RT_EOK)
    {
        return 1;
    }

    return 0;
}

__u32 esKRNL_GetTIDCur(void)
{
    rt_thread_t cur;
    __s32 idx;

    cur = rt_thread_self();
    if (cur == RT_NULL)
    {
        __err("error context.");
        software_break();
        return OS_TASK_NOT_EXIST;
    }

    // if module thread.
    if ((((__u32)cur->entry) & 0xf0000000) != 0xc0000000)
    {
        rt_enter_critical();
        idx = find_thread(EXEC_prioself);
        if (idx == -1)
        {
            rt_exit_critical();
            __err("cant find self thread id");
            return OS_TASK_NOT_EXIST;
        }
        rt_exit_critical();
#ifdef USE_MELIS_SHORT_THREAD_ID
        return idx;
#else
        return (__u32)melis_thread.thr_id[idx - 1].rt_thread_id
#endif
    }
    else  // kernel thread.
    {
        return (__u32)cur;
    }

    CODE_UNREACHABLE;
}

__u8 esKRNL_TaskResume(__u32 prio)
{
    __s32 idx;
    uint32_t level;

    level = rt_hw_interrupt_disable();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_hw_interrupt_enable(level);
        __err("fatal error, cant found thread");
        software_break();
        return (OS_TASK_NOT_EXIST);
    }

    if (melis_thread.thr_id[idx - 1].rt_thread_id)
    {
        if (rt_thread_resume(melis_thread.thr_id[idx - 1].rt_thread_id) != RT_EOK)
        {
            return OS_TASK_NOT_SUSPENDED;
        }

        melis_thread.thr_id[idx - 1].running = 1;

        rt_hw_interrupt_enable(level);
        /* re-schedule */
        rt_schedule();
    }
    else
    {
        rt_hw_interrupt_enable(level);
    }

    return OS_NO_ERR;
}

__u8 esKRNL_TaskSuspend(__u32 prio)
{
    __s32 idx;

#ifdef USE_MELIS_SHORT_THREAD_ID
    if ((prio > MELIS_THREAD_COUNT) || (prio <= 0))
    {
        __err("fatal error,  prio: %d.", prio);
        software_break();
        return OS_PRIO_INVALID;
    }
#else
    if (prio == (__u32)rt_thread_idle_gethandler())                    /* Not allowed to suspend idle task    */
    {
        return (OS_TASK_SUSPEND_IDLE);
    }
#endif

    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("fatal error, cant found thread");
        software_break();
        return (OS_TASK_NOT_EXIST);
    }

    if (melis_thread.thr_id[idx - 1].rt_thread_id == rt_thread_self())
    {
        melis_thread.thr_id[idx - 1].running = 0;
        rt_thread_suspend(rt_thread_self());
    }
    else if (melis_thread.thr_id[idx - 1].rt_thread_id)
    {
        melis_thread.thr_id[idx - 1].running = 0;
        rt_thread_suspend(melis_thread.thr_id[idx - 1].rt_thread_id);
    }
    else
    {
        __err("fatal error, should never happed!");
        software_break();
    }
    rt_exit_critical();

    /* re-schedule */
    rt_schedule();

    return (OS_NO_ERR);
}

void esMEMS_Pfree(void *mblk, __u32 npage)
{
    if (mblk == NULL)
    {
        return;
    }

    rt_free_align(mblk);
    return;
}

void *esMEMS_Realloc(void *heap, void *f, __u32 size)
{
    return rt_realloc(f, size);
}

void *esMEMS_Calloc(void *heap, __u32 n, __u32 m)
{
    return rt_calloc(n, m);
}

__s32 esMEMS_PhyAddrConti(void *mem, __u32 size)
{
#define DRAM_vBASE      CONFIG_DRAM_VIRTBASE
#define DRAM_SIZE       CONFIG_DRAM_SIZE

    __u32 start = (__u32)mem;

    if (size > DRAM_SIZE)
    {
        return 0;
    }

    if ((start >= DRAM_vBASE) && ((start + size) <= (DRAM_vBASE + DRAM_SIZE)))
    {
        return 1;
    }

    return 0;
}

static inline void dump_stack(rt_int32_t len)
{
    int i;
    rt_uint32_t sp;
    rt_uint32_t *buf;

    asm volatile("mov %0, r13\n":"=r"(sp));

    buf = (rt_uint32_t *)sp;

    rt_kprintf("\n\rdump stack memory,");
#ifdef CONFIG_RTTKERNEL
    rt_kprintf("thread %s, entry 0x%p:", rt_thread_self()->name, rt_thread_self()->entry);
#endif
    for (i = 0; i < len; i ++)
    {
        if ((i % 4) == 0)
        {
            rt_kprintf("\n\r0x%p: ", buf + i);
        }
        rt_kprintf("0x%08x ", buf[i]);
    }
    rt_kprintf("\n\r");

    return;
}

void *esMEMS_Palloc(__u32 npage, __u32 mode)
{
    void *ptr;

    ptr =  rt_malloc_align(npage * 1024, 1024);
    if (ptr == NULL)
    {
        __err("palloc failure.");
        esMEMS_Info();
        // software_break();
        return NULL;
    }

    /* palloc must be 1k aligned at least. */
    if (((__u32)ptr) & 0x3ff)
    {
        __err("palloc failure, not aligned to 1k. npage = %d, ptr = 0x%p, caller %p.", npage, ptr, __builtin_return_address(0));
        dump_stack(64);
        //software_break();
        return NULL;
    }

    /* not support more than 1k aligned attribute */
    if (mode != 0)
    {
        __err("palloc failure, mod not support! mode = %d, caller %p.", mode, __builtin_return_address(0));
        // software_break();
        return NULL;
    }

    return ptr;
}

__hdle esKRNL_MutexCreate(__u8 prio, __u8 *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_CREATE_ISR;
        }
        __err("create mutex in isr.");
        return NULL;
    }

    prio = prio;
    if (err)
    {
        *err = OS_NO_ERR;
    }

    return (__hdle)rt_mutex_create("melis_mutex", 0);
}

__u32 esKRNL_MutexDel(__hdle pevent, __u8 opt, __u8 *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_DEL_ISR;
        }
        __err("delete mutex in isr.");
        return (__u32)pevent;
    }

    opt = opt;

    rt_mutex_delete(pevent);

    if (err)
    {
        *err = OS_NO_ERR;
    }
    return 0;
}

void esKRNL_MutexPend(__hdle pevent, __u16 timeout, __u8 *err)
{
    rt_err_t ret;

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend mutex in isr.");
        return;
    }

    if (err)
    {
        *err = OS_NO_ERR;
    }
    timeout = timeout;

    ret = rt_mutex_take(pevent, RT_WAITING_FOREVER);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = OS_TIMEOUT;
        }
    }

    return;
}

__u8 esKRNL_MutexPost(__hdle pevent)
{
    rt_err_t err;
    if (rt_interrupt_get_nest() != 0)
    {
        __err("post mutex in isr.");
        return OS_ERR_POST_ISR;
    }

    err = rt_mutex_release(pevent);
    if (err != RT_EOK)
    {
        return OS_ERR_PIP_LOWER;
    }

    return OS_NO_ERR;
}

void esKRNL_TaskPrefEn(__u32 en)
{
#ifdef CONFIG_RT_USING_TASK_PERF_MONITOR
    void rt_perf_init(void);
    void rt_perf_exit(void);
    if (en)
    {
        rt_perf_init();
    }
    else
    {
        rt_perf_exit();
    }
#endif
    return;
}

void esKRNL_MemLeakChk(__u32 en)
{
    if (en)
    {
        epos_memleak_detect_enable();
    }
    else
    {
        epos_memleak_detect_disable();
    }

    return;
}

__s32 esMEMS_UnlockDCache(void *addr)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esMEMS_UnlockICache(void *addr)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esMEMS_LockDCache(void *addr, __u32 size)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esMEMS_LockICache(void *addr, __u32 size)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void esKRNL_FreePrio(__u32 prio)
{
    __err("not support syscall!");
    software_break();
}

__u8 esKRNL_GetPrio(__u8 priolevel)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__hdle esSVC_RegOpenNode(const char *node, __s32 mode)
{
    __err("not support syscall!");
    software_break();
    return NULL;
}

void esSVC_RegGetRootPath(char *path)
{
    __err("not support syscall!");
    software_break();
}
void esSVC_RegSetRootPath(const char *path)
{
    __err("not support syscall!");
    software_break();
}

__s32 esSVC_RegReg2Ini(const char *ini_file)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegIni2Reg(const char *ini_file)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegDeletePath(const char *path)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegCreatePath(const char *path)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void esKRNL_TaskChangePrio(void)
{
    __err("not support syscall!");
    software_break();
}

__s32 esSVC_RegClrError(__hdle handle)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegGetError(__hdle handle)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegGetKeyValue(__hdle hNode, const char *set_name, const char *key_name, char *key_value)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegSetKeyValue(__hdle hNode, const char *set_name, const char *key_name, const char *key_value)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegDeleteKey(__hdle hNode, const char *set_name, const char *key_name)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegCreateKey(__hdle hNode, const char *set_name, const char *key_name, const char *key_value)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegGetNextKey(__hdle hNode, const char *set_name, char *key_name)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegGetFirstKey(__hdle handle, const char *set_name)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegGetKeyCount(__hdle handle, const char *set_name, __u32 *count_p)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegDeleteSet(__hdle handle, const char *set_name)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegCreateSet(__hdle handle, const char *set_name)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegGetNextSet(__hdle handle, char *set_name)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegGetFirstSet(__hdle handle)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegGetSetCount(__hdle handle, __u32 *count_p)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegDeleteNode(const  char *node)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esSVC_RegCloseNode(__hdle handle)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__s32 esMEMS_HeapCreate(void *heapaddr, __u32 initnpage)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void esMEMS_HeapDel(void *heap)
{
    __err("not support syscall!");
    software_break();
    return;
}

void esMEMS_VMDelete(void *pBlk, __u32 npage)
{
    __err("not support syscall!");
    software_break();
    return;
}

__s32 esMEMS_VMCreate(void *pBlk, __u32 npage, rt_int8_t domain)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__u32 esKRNL_TmrRemainGet(__hdle ptmr)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__u8 esKRNL_QPostFront(__hdle pevent, void *msg)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__u8 esKRNL_QPostOpt(__hdle pevent, void *msg, __u8 opt)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__u8 esKRNL_MboxQuery(__hdle pevent, void *p_mbox_data)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

__u8 esKRNL_FlagNameGet(__hdle pgrp, __u8 *pname, __u8 *err)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void esKRNL_FlagNameSet(__hdle pgrp, __u8 *pname, __u8 *err)
{
    __err("not support syscall!");
    software_break();
}

__u32 esKRNL_FlagPendGetFlagsRdy(void)
{
    __err("not support syscall!");
    software_break();
    return 0;
}
__u8 esKRNL_MboxPostOpt(__hdle pevent, __u32 msg, __u8 opt)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

int printk(const char *fmt, ...)
{
    va_list args;
    rt_size_t length;
    static char printk_log_buf[RT_CONSOLEBUF_SIZE];
    char *p;
    int log_level;
    register rt_base_t level;

    va_start(args, fmt);
    level = rt_hw_interrupt_disable();
    rt_enter_critical();

    length = rt_vsnprintf(printk_log_buf, sizeof(printk_log_buf) - 1, fmt, args);
    if (length > RT_CONSOLEBUF_SIZE - 1)
    {
        length = RT_CONSOLEBUF_SIZE - 1;
    }
#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
    log_level = get_log_level();
    p = (char *)&printk_log_buf;
    if (p[0] != '<' || p[1] < '0' || p[1] > '7' || p[2] != '>')
    {
        if (log_level > (OPTION_LOG_LEVEL_CLOSE))
        {
            rt_kprintf("%s", (char *)&printk_log_buf);
        }
    }
    else
    {
        if (log_level >= (p[1] - '0'))
        {
            rt_kprintf("%s", (char *)&printk_log_buf[3]);
        }
    }
#else
    rt_kprintf("%s", (char *)&printk_log_buf);
#endif
    rt_exit_critical();
    rt_hw_interrupt_enable(level);
    va_end(args);

    return length;
}

void *k_malloc(rt_uint32_t sz) __attribute__((alias("rt_malloc")));
void k_free(void *ptr) __attribute__((alias("rt_free")));
