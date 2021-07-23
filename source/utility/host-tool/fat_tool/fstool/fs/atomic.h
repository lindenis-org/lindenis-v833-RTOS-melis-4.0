/*
 *  linux/include/asm-arm/atomic.h
 *
 *  Copyright (C) 1996 Russell King.
 *  Copyright (C) 2002 Deep Blue Solutions Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_ARM_ATOMIC_H
#define __ASM_ARM_ATOMIC_H

#include "fsys_types.h"
#include "fsys_libs.h"

extern __u32 CPU_SR_Save(void);
extern void CPU_SR_Restore(__u32 x);

typedef struct
{
    volatile int counter;
} atomic_t;

#define ATOMIC_INIT(i)  { (i) }

#define atomic_read(v)  ((v)->counter)

#define atomic_set(v,i) (((v)->counter) = (i))

static __inline int atomic_add_return(int i, atomic_t *v)
{
    unsigned long flags;
    int val;

    ENTER_CRITICAL(flags);
    val = v->counter;
    v->counter = val += i;
    EXIT_CRITICAL(flags);

    return val;
}

static __inline int atomic_sub_return(int i, atomic_t *v)
{
    unsigned long flags;
    int val;

    ENTER_CRITICAL(flags);
    val = v->counter;
    v->counter = val -= i;
    EXIT_CRITICAL(flags);

    return val;
}

static __inline int atomic_cmpxchg(atomic_t *v, int old, int new)
{
    int ret;
    unsigned long flags;

    ENTER_CRITICAL(flags);
    ret = v->counter;
    if (ret == old)
        v->counter = new;
    EXIT_CRITICAL(flags);

    return ret;
}

static __inline void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
    unsigned long flags;

    ENTER_CRITICAL(flags);
    *addr &= ~mask;
    EXIT_CRITICAL(flags);
}

//#define atomic_xchg(v, new) (xchg(&((v)->counter), new))

static __inline int atomic_add_unless(atomic_t *v, int a, int u)
{
    int c, old;

    c = atomic_read(v);
    while (c != u && (old = atomic_cmpxchg((v), c, c + a)) != c)
        c = old;
    return c != u;
}
#define atomic_inc_not_zero(v) atomic_add_unless((v), 1, 0)

#define atomic_add(i, v)    (void) atomic_add_return(i, v)
#define atomic_inc(v)       (void) atomic_add_return(1, v)
#define atomic_sub(i, v)    (void) atomic_sub_return(i, v)
#define atomic_dec(v)       (void) atomic_sub_return(1, v)

#define atomic_inc_and_test(v)  (atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)  (atomic_sub_return(1, v) == 0)
#define atomic_inc_return(v)    (atomic_add_return(1, v))
#define atomic_dec_return(v)    (atomic_sub_return(1, v))
#define atomic_sub_and_test(i, v) (atomic_sub_return(i, v) == 0)

#define atomic_add_negative(i,v) (atomic_add_return(i, v) < 0)

#define atomic_dec_and_lock(atomic,lock) atomic_dec_and_test(atomic)

#endif

