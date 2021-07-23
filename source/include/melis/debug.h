/*
 * =====================================================================================
 *
 *       Filename:  debug.h
 *
 *    Description:  software breakpoint definition.
 *
 *        Version:  2.0
 *         Create:  2017-11-02 10:24:42
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-02-28 10:52:52
 *
 * =====================================================================================
 */

#ifndef __MELIS_DEBUG_H__
#define __MELIS_DEBUG_H__

#include <arch.h>

/*
 * gcc -c -Q -O2 --help=optimizers > /tmp/O2-option
 * gcc -c -Q -Os --help=optimizers > /tmp/Os-option
 */
#define OPTIMILIZE_O0_LEVEL   __attribute__((optimize("O0")))
#define OPTIMILIZE_O1_LEVEL   __attribute__((optimize("O1")))
#define OPTIMILIZE_O2_LEVEL   __attribute__((optimize("O2")))
#define OPTIMILIZE_O3_LEVEL   __attribute__((optimize("O3")))
#define OPTIMILIZE_Os_LEVEL   __attribute__((optimize("Os")))

#define noinline              __attribute__((noinline))

/*
 *  ------------------------------------------------------------------------ *
 *                                                                           *
 *   Software Breakpoint code                                                *
 *   Uses inline assembly command                                            *
 *                                                                           *
 *  ------------------------------------------------------------------------ *
 */
#define armv5_break(im)   (0xe1200070 | ((im & 0xfff0) << 8) | (im & 0xf))
#define software_break(...) do { \
        unsigned long _cpsr;     \
        _cpsr = awos_arch_disable_interrupt();   \
        asm volatile ("bkpt #0": : :"memory");   \
        awos_arch_restore_interrupt(_cpsr);      \
    } while(0)

void gdb_start(void);
#endif /*__MELIS_DEBUG_H__*/

