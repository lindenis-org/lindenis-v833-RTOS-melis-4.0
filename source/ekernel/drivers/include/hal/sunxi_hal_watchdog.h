/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2014
 * Chen-Yu Tsai <wens@csie.org>
 *
 * Watchdog register definitions
 */

#ifndef _SUNXI_HAL_WATCHDOG_H_
#define _SUNXI_HAL_WATCHDOG_H_

#include <typedef.h>

#define readl(addr)     (*((volatile unsigned long  *)(addr)))
#define writel(v, addr) (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

#define WTD_BASE  0x030090a0
#define WTD_TIMEOUT 16

#define WTD_CTRL_RESTART    (0x1 << 0)
#define WTD_CTRL_KEY        (0x0a57 << 1)

#define WTD_CFG_RESET       (0x1)
#define WTD_MODE_EN     (0x1)


/* watchdog register offset */
#define WTD_IRQ_EN   (WTD_BASE + 0X00)
#define WTD_STA      (WTD_BASE + 0X04)
#define WTD_CTL      (WTD_BASE + 0X10)
#define WTD_CFG      (WTD_BASE + 0X14)
#define WTD_MODE     (WTD_BASE + 0X18)


struct hal_sunxi_wtd{
	volatile u32 irq_en;    //0x00
	volatile u32 sta;   //0x04
	volatile u32 rsl[2];    
	volatile u32 ctl;    //0x10
	volatile u32 cfg;    //0x14
	volatile u32 mode;   //0x18
};



void hal_watchdog_disable(void);
void hal_watchdog_reset(int timeout);
void hal_watchdog_restart(void);
void hal_watchdog_info(void);
void hal_watchdog_info(void);


#endif /* _SUNXI_HAL_WATCHDOG_H_ */
