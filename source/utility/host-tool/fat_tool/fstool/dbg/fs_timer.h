#ifndef    _FS_USB_TIME_H_
#define    _FS_USB_TIME_H_


#include "buffer_head.h"
#include "list.h"
#include "page_pool.h"
#include "fsys_debug.h"

/*--------------------------- debug ------------------------*/

struct timer_cfg_s
{
    int on;
};

typedef enum ttype_e
{
    diodevicetime = 0,
    bufdevicetime,
    memcpytime,
    getblktime,
    fatcachetime,
    scanfattime,
    oneaccesstime,
    bmaptime,
    hashtime,
    shrinkcachetime,
    partiotime,

    invalidtime = 0x80,
    alltime = 0xff,
} ttype_t;

typedef struct item_s
{
    ttype_t      type;
    __u32        value;
    __u32        start;
    const char  *info;
} titem_t;

typedef struct timers_s
{
    int             count;
    titem_t         item[12];
} timers_t;

#ifndef  simulate
extern void debug_timerclr(ttype_t target);
extern void debug_timerstart(ttype_t target);
extern __u32 debug_timerend(ttype_t target);
extern void debug_timerprint(char *fmt, ...);
extern void debug_timershow(ttype_t target);

extern BOOL fs_timer_parse_option(const char *option);

#else

/* no-op */
static void debug_timerclr(ttype_t target)
{
    return;
}
static void debug_timerstart(ttype_t target)
{
    return;
}
static __u32 debug_timerend(ttype_t target)
{
    return 0;
}
static void debug_timerprint(char *fmt, ...)
{
    return;
}
static void debug_timershow(ttype_t target)
{
    return;
}

static BOOL fs_timer_parse_option(const char *option)
{
    return 0;
}


#endif  /* simulate */

#endif

