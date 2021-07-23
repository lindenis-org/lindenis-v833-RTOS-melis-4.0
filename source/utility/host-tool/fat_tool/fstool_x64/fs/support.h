/*
 * support.h - Various useful things. Part of the Linux-NTFS project.
 *
 * Copyright (c) 2000-2004 Anton Altaparmakov
 * Copyright (c)      2006 Szabolcs Szakacsits
 * Copyright (c)      2006 Yura Pakhuchiy
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the Linux-NTFS
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __SUPPORT_H__
#define __SUPPORT_H__

//#include <stddef.h>
#include "sys_fsys_i.h"
/*
 * Generic macro to convert pointers to values for comparison purposes.
 */
#ifndef p2n
#define p2n(p)      ((ptrdiff_t)((ptrdiff_t*)(p)))
#endif

/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#ifndef min
#define min(x,y) ({ \
    typeof(x) _x = (x); \
    typeof(y) _y = (y); \
    (void) (&_x == &_y);        \
    _x < _y ? _x : _y; })
#endif

#ifndef max
#define max(x,y) ({ \
    typeof(x) _x = (x); \
    typeof(y) _y = (y); \
    (void) (&_x == &_y);        \
    _x > _y ? _x : _y; })
#endif

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max at all, of course.
 */
#define min_t(type,x,y) \
    ({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
#define max_t(type,x,y) \
    ({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })

#ifndef BITS_PER_LONG
#define BITS_PER_LONG 32
#endif

#ifndef ALIGN
#define ALIGN(val,align)    (((val) + ((align) - 1)) & ~((align) - 1))
#endif

#ifndef LONG_ALIGN
#define LONG_ALIGN(x) (((x)+(sizeof(long))-1)&~((sizeof(long))-1))
#endif

#ifndef INT_MAX
#define INT_MAX     ((int)(~0U>>1))
#endif

#ifndef INT_MIN
#define INT_MIN     (-INT_MAX - 1)
#endif

#ifndef UINT_MAX
#define UINT_MAX    (~0U)
#endif

#ifndef LONG_MAX
#define LONG_MAX    ((long)(~0UL>>1))
#endif

#ifndef LONG_MIN
#define LONG_MIN    (-LONG_MAX - 1)
#endif

#ifndef ULONG_MAX
#define ULONG_MAX   (~0UL)
#endif

#ifndef LLONG_MAX
#define LLONG_MAX   ((long long)(~0ULL>>1))
#endif

#ifndef LLONG_MIN
#define LLONG_MIN   (-LLONG_MAX - 1)
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX  (~0ULL)
#endif

typedef enum
{
#ifndef FALSE
    FALSE = 0,
#endif
#ifndef NO
    NO = 0,
#endif
#ifndef ZERO
    ZERO = 0,
#endif
#ifndef TRUE
    TRUE = 1,
#endif
#ifndef YES
    YES = 1,
#endif
#ifndef ONE
    ONE = 1,
#endif
#ifndef OK
    OK = 0,
#endif
#ifndef FAIL
    FAIL = -1,
#endif
} BOOL;

/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#define typecheck(type,x) \
({  type __dummy; \
    typeof(x) __dummy2; \
    (void)(&__dummy == &__dummy2); \
    1; \
})
//#define offsetof(TYPE, MEMBER) ((unsigned int) &((TYPE *)0)->MEMBER)
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

static __inline int is_power_of_2(unsigned long n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

/*
 * Round up and down @num to 2 in power of @order.
 */
#define ROUND_UP(num,order) (((num) + ((1 << (order)) - 1)) & \
                ~((1 << (order)) - 1))
#define ROUND_DOWN(num,order)   ((num) & ~((1 << (order)) - 1))

/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static __inline int test_bit(int nr, long *addr)
{
    int mask;

    addr += nr >> 5;
    mask = 1 << (nr & 0x1f);
    return ((mask & *addr) != 0);
}

/*
 * These functions are the basis of our bit ops.
 *
 * First, the atomic bitops. These use native endian.
 */
static __inline void set_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    ENTER_CRITICAL(flags);
    *p |= mask;
    EXIT_CRITICAL(flags);
}

static __inline void clear_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    ENTER_CRITICAL(flags);
    *p &= ~mask;
    EXIT_CRITICAL(flags);
}

static __inline void change_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    ENTER_CRITICAL(flags);
    *p ^= mask;
    EXIT_CRITICAL(flags);
}

static __inline int
test_and_set_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned int res;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    ENTER_CRITICAL(flags);
    res = *p;
    *p = res | mask;
    EXIT_CRITICAL(flags);

    return res & mask;
}

static __inline int
test_and_clear_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned int res;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    ENTER_CRITICAL(flags);
    res = *p;
    *p = res & ~mask;
    EXIT_CRITICAL(flags);

    return res & mask;
}

static __inline int
test_and_change_bit(unsigned int bit, volatile unsigned long *p)
{
    unsigned long flags;
    unsigned int res;
    unsigned long mask = 1UL << (bit & 31);

    p += bit >> 5;

    ENTER_CRITICAL(flags);
    res = *p;
    *p = res ^ mask;
    EXIT_CRITICAL(flags);

    return res & mask;
}

/* -------------------------------- jiffies -----------------------------*/
#define HZ 100
#define jiffies ((unsigned long)esKRNL_Time())

/*
 *  These inlines deal with timer wrapping correctly. You are
 *  strongly encouraged to use them
 *  1. Because people otherwise forget
 *  2. Because if the timer wrap changes in future you won't have to
 *     alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#define time_after(a,b)     \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((int)(b) - (int)(a) < 0))
#define time_before(a,b)    time_after(b,a)

#define time_after_eq(a,b)  \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((int)(a) - (int)(b) >= 0))
#define time_before_eq(a,b) time_after_eq(b,a)

#endif /* defined __SUPPORT_H__ */
