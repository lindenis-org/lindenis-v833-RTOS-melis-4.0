/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some init for sunxi platform.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <private_boot0.h>
#include <arch/clock.h>
#include <arch/uart.h>

int sunxi_board_init(void)
{
	printf("board init start\n");
	sunxi_board_pll_init();
	axp_init(0);
	set_sys_voltage(CONFIG_SUNXI_SYS_VOL);
	printf("board init ok\n");
	return 0;
}

int  sunxi_deassert_arisc(void)
{
	printf("set arisc reset to de-assert state\n");
	{
		volatile unsigned long value;
		value = readl(SUNXI_RCPUCFG_BASE + 0x0);
		value &= ~1;
		writel(value, SUNXI_RCPUCFG_BASE + 0x0);
		value = readl(SUNXI_RCPUCFG_BASE + 0x0);
		value |= 1;
		writel(value, SUNXI_RCPUCFG_BASE + 0x0);
	}

	return 0;
}

int sunxi_board_exit(void)
{
	return 0;
}
