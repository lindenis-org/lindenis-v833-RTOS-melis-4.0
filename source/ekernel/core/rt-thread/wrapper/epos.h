/*
 * ===========================================================================================
 *
 *       Filename:  epos.h
 *
 *    Description:  epos.c typedef.
 *
 *        Version:  Melis3.0
 *         Create:  2018-03-30 20:34:51
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-09-28 10:50:56
 *
 * ===========================================================================================
 */

#ifndef EPOS_H
#define EPOS_H

#include <rthw.h>
#include <kapi.h>
#include <list.h>
#include <debug.h>

#define  USE_MELIS_SHORT_THREAD_ID
#define  MELIS_TASK_PREFIX      "mlstsk_"
#define  MELIS_THREAD_COUNT     (255u)
#define  MEMLEAK_STK_DUMPSZ     (1024)
#define  MAX_ICACHE_LOCKED_WAY  3                    /* max locked d-cache ways number, * NOTE : must reserve one way for normal use */
#define  MAX_DCACHE_LOCKED_WAY  3
#define  CONFIG_MEMORY_LEAK_BACKTRACE_LEVEL 16

void UART_PutStr(const char *str);
typedef struct
{
    rt_mailbox_t   mb_in;
    rt_mailbox_t   mb_out;
    __u32          depth;
    __u32          bufsize;
    __u32          msgsize;
    __u32          dbufmod;
    __u32          mbufmod;
    __u32          err;
    void          *dbufaddr;
    void          *mbufaddr;
} __krnl_skt_t;

typedef struct  __CACHE_WAY
{
    void    *addr;       /* start address of lock    */
    __s32   lockbit;    /* locked way bit value     */
} __cache_way_t;

typedef struct thread_id
{
    __u8        running;
    rt_thread_t rt_thread_id;
} thread_id_t;

typedef struct melis_thread_obj
{
    thread_id_t thr_id[MELIS_THREAD_COUNT];
} melis_thread_t;

typedef struct melis_malloc_context
{
    struct list_head    list;
    __s32               ref_cnt;
    __s32               open_flag;
} melis_malloc_context_t;

typedef struct melis_heap_buffer_node
{
    struct list_head  i_list;
    rt_tick_t         tick;
    void             *vir;
    __u32             size;
    __u32             entry;
    __s8             *name;
    //__u8              stack[MEMLEAK_STK_DUMPSZ];
    __u8              *stk;
#ifdef CONFIG_DEBUG_BACKTRACE
    void              *caller[CONFIG_MEMORY_LEAK_BACKTRACE_LEVEL];
#endif
} melis_heap_buffer_node_t;
#endif  /*EPOS_H*/
