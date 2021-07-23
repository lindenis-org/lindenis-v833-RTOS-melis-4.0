//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                                Scott System                                              //
//                                                                                                          //
//                               (c) Copyright 2006-2007, Scottyu China                                     //
//                                                                                                          //
//                                           All Rights Reserved                                            //
//                                                                                                          //
// File    : basetypes.h                                                                                    //
// By      : scottyu                                                                                        //
// Version : V1.00                                                                                          //
// Time    : 2007-12-13 15:50:23                                                                            //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
// HISTORY                                                                                                  //
//                                                                                                          //
// 1 2007-12-13 15:50:23                                                                                    //
// 2 2017-12-20                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//


//!定义统一的基本数据类型，采用自定义的基本数据类型来编写代码，以提高移植性
//!其他文件都需要包含本文件，不要采用其他的基本数据类型


#ifndef __ANET_BASE_TYPES___H____
#define __ANET_BASE_TYPES___H____ 1

//------------------------------------------------------------------------------------------------------------
//!1.00版本
//------------------------------------------------------------------------------------------------------------
#define BASE_TYPE_VER       0x0100
#include <string>
#include <stdio.h>

#define TRACE printf

typedef char TCHAR;

#define _MAX_PATH 260
#define _MAX_FNAME 256
#define _MAX_EXT 256
#define _MAX_DRIVE 3
#define _MAX_DIR 256
#define MAX_PATH 260

typedef void *HANDLE;

//基本数据类型定义
#ifndef OK
#define OK                  0
#endif //OK

#ifndef NULL
#define NULL 0
#endif //NULL

#if 1

//typedef char              s8;
//typedef char              __s8;
//typedef char              int8;

typedef signed char         s8;
typedef signed char         __s8;
typedef signed char         int8;

typedef unsigned char       u8;
typedef unsigned char       __u8;
typedef unsigned char       uint8;

typedef short int           s16;
typedef short int           __s16;
typedef short int           int16;

typedef unsigned short      u16;
typedef unsigned short      __u16;
typedef unsigned short      uint16;

typedef int                 s32;
typedef int                 __s32;
typedef int                 int32;

typedef unsigned int        u32;
typedef unsigned int        __u32;
typedef unsigned int        uint32;


typedef float               f32;
typedef double              df;

typedef void               *p32;

#else

//------------------------------------------------------------------------------------------------------------
//!有符号整数
//------------------------------------------------------------------------------------------------------------
#ifndef s8
#define s8                  char
#endif //s8


#ifndef s16
#define s16                 short int
#endif //s16

#ifndef s32
#define s32                 int
#endif //s32


#ifndef __s8
#define __s8                char
#endif //__s8


#ifndef __s16
#define __s16               short int
#endif //__s16


#ifndef __s32
#define __s32               int
#endif //__s32

#ifndef int8
#define int8                char
#endif //int8

#ifndef int16
#define int16               short int
#endif //int16


#ifndef int32
#define int32               int
#endif //int32

//------------------------------------------------------------------------------------------------------------
//!无符号整数
//------------------------------------------------------------------------------------------------------------

#ifndef u8
#define u8                  unsigned char
#endif //u8


#ifndef u16
#define u16                 unsigned short
#endif //u16

#ifndef u32
#define u32                 unsigned int
#endif //u32


#ifndef __u8
#define __u8                unsigned char
#endif //__u8


#ifndef __u16
#define __u16               unsigned short
#endif //__u16


#ifndef __u32
#define __u32               unsigned int
#endif //__u32



#ifndef uint8
#define uint8               unsigned char
#endif //uint8

#ifndef uint16
#define uint16              unsigned short int
#endif //uint16


#ifndef uint32
#define uint32              unsigned int
#endif //uint32

//------------------------------------------------------------------------------------------------------------
//!浮点数
//------------------------------------------------------------------------------------------------------------

#ifndef f32
#define f32                 float
#endif //f32

#ifndef df
#define df                  double
#endif //df

//------------------------------------------------------------------------------------------------------------
//!无类型
//------------------------------------------------------------------------------------------------------------
#ifndef p32
#define p32                 void *
#endif //p32

#endif // 1
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#define TYPE_Invalid        0x00
#define TYPE_S8             0x10
#define TYPE_S16            0x11
#define TYPE_S32            0x12
#define TYPE_u8             0x20
#define TYPE_u16            0x21
#define TYPE_u32            0x22
#define TYPE_f32            0x30
#define TYPE_df             0x31
#define TYPE_p32            0x40
#define TYPE_v32            0x41


//------------------------------------------------------------------------------------------------------------
///为了提高数据抽象及移植性设计的通用数据类型
//------------------------------------------------------------------------------------------------------------
typedef struct tag_UniVar
{
    u32 type;
    u32 res;
    union
    {
        s8  s8Val;
        s16 s16Val;
        s32 s32Val;
        u8  u8Val;
        u16 u16Val;
        u32 u32Val;
        f32 f32Val;
        df  dfVal;
        p32 p32Val;
        //v32 v32Val;
    };
} __attribute__((packed)) UniVar_t;


#define SIZE_4          4
#define SIZE_8          8
#define SIZE_16         16
#define SIZE_32         32
#define SIZE_64         64
#define SIZE_128        128
#define SIZE_256        256
#define SIZE_512        512
#define SIZE_1K         1024
#define SIZE_2K         2048
#define SIZE_4K         4096
#define SIZE_8K         8192


#define SIZE_32K        0x8000

#define SIZE_1M         1048576


/*
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif// offsetof
*/

#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

/*
#include <stdio.h>
void main()
{
    UniVar_t  var;
    var.res = 0;
    printf("sizeof UniVar_t\t=%d\n", sizeof(var));
    printf("sizeof type\t=%d\toffset=%d\n", sizeof(var.type), offsetof(UniVar_t, type));
    printf("sizeof res\t=%d\toffset=%d\n", sizeof(var.res), offsetof(UniVar_t, res));
    var.type = TYPE_S8;
    var.s8Val= 255;
    printf("sizeof s8\t=%d\toffset=%d\n", sizeof(var.s8Val), offsetof(UniVar_t, s8Val));
    printf("sizeof s16\t=%d\toffset=%d\n", sizeof(var.s16Val), offsetof(UniVar_t, s16Val));
    printf("sizeof s32\t=%d\toffset=%d\n", sizeof(var.s32Val), offsetof(UniVar_t, s32Val));
    printf("sizeof u8\t=%d\toffset=%d\n", sizeof(var.u8Val), offsetof(UniVar_t, u8Val));
    printf("sizeof u16\t=%d\toffset=%d\n", sizeof(var.u16Val), offsetof(UniVar_t, u16Val));
    printf("sizeof u32\t=%d\toffset=%d\n", sizeof(var.u32Val), offsetof(UniVar_t, u32Val));
    printf("sizeof f32\t=%d\toffset=%d\n", sizeof(var.f32Val), offsetof(UniVar_t, f32Val));
    printf("sizeof df\t=%d\toffset=%d\n", sizeof(var.dfVal), offsetof(UniVar_t, dfVal));
    printf("sizeof p32\t=%d\toffset=%d\n", sizeof(var.p32Val), offsetof(UniVar_t, p32Val));
    system("pause");
}
*/

/*
#ifdef ___GARY_TEST___

#define SZ_512       0x00000200U
#define SZ_1K        0x00000400U
#define SZ_2K        0x00000800U
#define SZ_4K        0x00001000U
#define SZ_8K        0x00002000U
#define SZ_16K       0x00004000U
#define SZ_32K       0x00008000U
#define SZ_64K       0x00010000U
#define SZ_128K      0x00020000U
#define SZ_256K      0x00040000U
#define SZ_512K      0x00080000U
#define SZ_1M        0x00100000U
#define SZ_2M        0x00200000U
#define SZ_4M        0x00400000U
#define SZ_8M        0x00800000U
#define SZ_16M       0x01000000U
#define SZ_32M       0x02000000U
#define SZ_64M       0x04000000U
#define SZ_128M      0x08000000U
#define SZ_256M      0x10000000U
#define SZ_512M      0x20000000U
#define SZ_1G        0x40000000U
#define SZ_2G        0x80000000U

#define SZ_4G        0x0100000000ULL
#define SZ_8G        0x0200000000ULL
#define SZ_16G       0x0400000000ULL
#define SZ_32G       0x0800000000ULL
#define SZ_64G       0x1000000000ULL




#ifndef NULL
#define NULL                  0
#endif //NULL

#define RSVD_VALUE            0


#define COMPILE_TIME_ASSERT(expr)       typedef char assert_type[(expr) ? 1 : -1]


#define set_bit( x, y )      ( (x) |=  ( 1U << (y) ) )
#define clear_bit( x, y )    ( (x) &= ~( 1U << (y) ) )
#define reverse_bit( x, y )  ( (x) ^=  ( 1U << (y) ) )
#define test_bit( x, y )     ( (x)  &  ( 1U << (y) ) )

#ifndef min
#define min( x, y )          ( (x) < (y) ? (x) : (y) )
#endif //min

#ifndef max
#define max( x, y )          ( (x) > (y) ? (x) : (y) )
#endif //max

#endif //___GARY_TEST___
*/
#endif //!__ANET_BASE_TYPES___H____

