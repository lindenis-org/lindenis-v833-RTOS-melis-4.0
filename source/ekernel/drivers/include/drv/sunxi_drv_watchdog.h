/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2014
 * Chen-Yu Tsai <wens@csie.org>
 *
 * Watchdog register definitions
 */

#ifndef _SUNXI_DRV_WATCHDOG_H_
#define _SUNXI_DRV_WATCHDOG_H_

void drv_watchdog_info(void);
void drv_watchdog_disable(void);
void drv_watchdog_reset(int timeout);
void reset_cpu(void);

#endif /* _SUNXI_DRV_WATCHDOG_H_ */
