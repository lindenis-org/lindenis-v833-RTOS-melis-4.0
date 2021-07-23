/*
 * ===========================================================================================
 *
 *       Filename:  sunxi_hal_common.h
 *
 *    Description:  Add sunxi hal common header file, common defitions used by all hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-14 11:38:34
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-04-13 09:07:09
 *
 * ===========================================================================================
 */
#ifndef SUNXI_HAL_COMMON_H
#define SUNXI_HAL_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef uint32_t  u32;
typedef uint8_t   u8;
#define HAL_ARG_UNUSED(NAME)   (void)(NAME)

// Exception Dealt With.
#define BUG() do {                                                             \
        printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
        while(1);                                                              \
    } while (0)

#ifndef BUG_ON
#define BUG_ON(condition)   do { if (unlikely(condition)) BUG(); } while (0)
#define WARN_ON(condition)  ({                                          \
        int __ret_warn_on = !!(condition);                              \
        unlikely(__ret_warn_on);                                        \
    })
#endif

#ifndef WARN
#define WARN(condition, format...) ({                                   \
        int __ret_warn_on = !!(condition);                              \
        if(__ret_warn_on)                                               \
            printf(format);                                             \
        unlikely(__ret_warn_on);                                        \
    })
#endif

#define in_interrupt(...)       rt_interrupt_get_nest()
#define in_nmi(...)             (0)

#define hal_assert(ex)                                                  \
    if (!(ex))                                                          \
    {                                                                   \
        printf("%s line %d, fatal error.\n", __func__, __LINE__);       \
        while(1);                                                       \
    }


// common register access operation.
#define hal_readb(reg)          (*(volatile uint8_t  *)(reg))
#define hal_readw(reg)          (*(volatile uint16_t *)(reg))
#define hal_readl(reg)          (*(volatile uint32_t *)(reg))
#define hal_writeb(value,reg)   (*(volatile uint8_t  *)(reg) = (value))
#define hal_writew(value,reg)   (*(volatile uint16_t *)(reg) = (value))
#define hal_writel(value,reg)   (*(volatile uint32_t *)(reg) = (value))

// version combine.
#define SUNXI_HAL_VERSION_MAJOR_MINOR(major, minor)     (((major) << 8) | (minor))

typedef struct sunxi_hal_version
{
    // API version NO.
    uint16_t api;

    // Driver version NO.
    uint16_t drv;
} sunxi_hal_version_t;

// General return code of hal driver.
#define SUNXI_HAL_OK                     0UL
// Unspecified error.
#define SUNXI_HAL_ERROR                 -1UL
// Hal is busy.
#define SUNXI_HAL_ERROR_BUSY            -2UL
// Timout occured.
#define SUNXI_HAL_ERROR_TIMEOUT         -3UL
// Operaion not supported.
#define SUNXI_HAL_ERROR_UNSUPOT         -4UL
// Parameter error.
#define SUNXI_HAL_ERROR_PARAERR         -5UL
// Start of driver specific errors.
#define SUNXI_HAL_ERROR_DRVSPECIFIC     -6UL

// brief General power states
typedef enum sunxi_hal_power_state
{
    ///< Power off: no operation possible
    SUSNXI_HAL_POWER_OFF,
    ///< Low Power mode: retain state, detect and signal wake-up events
    SUSNXI_HAL_POWER_LOW,
    ///< Power on: full operation at maximum performance
    SUSNXI_HAL_POWER_FULL
} sunxi_hal_power_state_e;

#define isb(option)  __asm__ __volatile__ ("isb " #option : : : "memory")
#define dsb(option)  __asm__ __volatile__ ("dsb " #option : : : "memory")
#define dmb(option)  __asm__ __volatile__ ("dmb " #option : : : "memory")

typedef int32_t (*poll_wakeup_func)(int32_t dev_id, short key);

typedef struct _sunxi_hal_poll_ops
{
    int32_t (* check_poll_state) (int32_t dev, short key);
    int32_t (* hal_poll_wakeup) (int32_t dev, short key);
    int32_t (* register_poll_wakeup) (poll_wakeup_func poll_wakeup);
} sunxi_hal_poll_ops;

#ifdef __cplusplus
}
#endif

#endif  /*SUNXI_HAL_COMMON_H*/

