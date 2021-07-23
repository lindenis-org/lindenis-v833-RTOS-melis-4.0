/*
 * fstime.h
 * fs timer support under epos platform
 *
 */
#ifndef     __FS_TIME_H__
#define     __FS_TIME_H__

#include "fstypedef.h"      /* just for type : time_t */

/* Parameters used to convert the timespec values: */
#define MSEC_PER_SEC    1000L
#define USEC_PER_MSEC   1000L
#define NSEC_PER_USEC   1000L
#define NSEC_PER_MSEC   1000000L
#define USEC_PER_SEC    1000000L
#define NSEC_PER_SEC    1000000000L
#define FSEC_PER_SEC    1000000000000000L

struct ktimestamp
{
    __u16       year;
    __u8        month;
    __u8        day;
    __u8        hour;
    __u8        minute;
    __u8        second;
    __u8        milliseconds;
};

//modify by gongxf
//struct timespec {
//  time_t  tv_sec;     /* seconds */
//  long    tv_nsec;    /* nanoseconds */
//};

struct timezone
{
    int tz_minuteswest;   /* minutes west of Greenwich */
    int tz_dsttime;       /* type of dst correction */
};

unsigned long   current_seconds(void);   /* get current seconds */
struct timespec current_timespec(void);  /* get current timespec */

#define CURRENT_TIME        (current_timespec())
#define CURRENT_TIME_SEC    ((struct timespec) { current_seconds(), 0 })

#endif      /* __FS_TIME_H__ */
