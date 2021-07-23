
#ifndef     __FSYS_TYPES_H__
#define     __FSYS_TYPES_H__

/*
********************************************************************************
*                         常量定义
********************************************************************************
*/
#define  EPDK_OK        0                           /* 代表成功  */
#define  EPDK_FAIL      (-1)                        /* 代表失败  */
#define  EPDK_TRUE      1                           /* 真        */
#define  EPDK_FALSE     0                           /* 假        */

#define  EPDK_DISABLED  0
#define  EPDK_ENABLED   1
#define  EPDK_NO        0
#define  EPDK_YES       1
#define  EPDK_OFF       0
#define  EPDK_ON        1
#define  EPDK_CLR       0
#define  EPDK_SET       1

#ifndef  NULL
#define  NULL            0                          /* 指针空    */
#endif   /* NULL */

/*
********************************************************************************
*                      DATA TYPES
********************************************************************************
*/
#undef  __u64
typedef unsigned long long  __u64;
#undef  __u32
typedef unsigned int        __u32;
typedef unsigned int        u32;
#undef  __u16
typedef unsigned short      __u16;
#undef  __u8
typedef unsigned char       __u8;
typedef unsigned char       u8;
#undef  __s64
typedef signed long long    __s64;
#undef  __s32
typedef signed int          __s32;
typedef signed int          s32;
#undef  __s16
typedef signed short        __s16;
#undef  __s8
typedef signed char         __s8;
typedef signed char         s8;
#undef  __bool
typedef signed char         __bool;
#ifndef __HDLE
#define __HDLE
//#undef    __hdle            //modify by gongxf 2011.4.12
typedef void               *__hdle;
#endif  //__HDLE
#undef  __size
typedef unsigned int        __size;
#undef  __size_t
typedef unsigned int        __size_t;

/* other struct under ePDK */
typedef void *__krnl_event_t;

#define  NEGATIVE       1
#define  POSITIVE       0
#define  HEX_x          'x'
#define  HEX_X          'X'

#endif      /* __FSYS_TYPES_H__ */

