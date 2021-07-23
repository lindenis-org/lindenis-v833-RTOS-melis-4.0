/*
 * source/ekernel/drivers/drv/source/disp2/soc/platform_resource/platform_resource.c
 *
 * Copyright (c) 2007-2020 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <hal_clk.h>
#include "platform_resource.h"

extern u32 g_irq_no[];
extern u32 g_reg_base[];
extern struct clk_info_t g_clk_no[];
extern u32 g_irq_no_len;
extern u32 g_reg_base_len;
extern u32 g_clk_no_len;

s32 plat_get_reg_base(u32 index, u32 *data)
{
	if (index >= g_reg_base_len || !data)
		return -1;
	*data = g_reg_base[index];
	return 0;
}

s32 plat_get_irq_no(u32 index, u32 *data)
{
	if (index >= g_irq_no_len || !data)
		return -1;
	*data = g_irq_no[index];
	return 0;
}

s32 plat_get_clk(u32 index, hal_clk_id_t *data)
{
	if (index >= g_clk_no_len || !data)
		return -1;
	*data = g_clk_no[index].clk;
	return 0;
}

s32 plat_get_clk_parent(hal_clk_id_t clk, hal_clk_id_t *parent)
{
	u32 i = 0;

	for (i = 0; i < g_clk_no_len; ++i) {
		if (g_clk_no[i].clk == clk) {
			*parent = g_clk_no[i].clk_parent;
			break;
		}
	}

	return (i == g_clk_no_len) ? -1 : 0;
}
