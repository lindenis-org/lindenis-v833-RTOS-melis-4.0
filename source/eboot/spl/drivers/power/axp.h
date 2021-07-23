/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 * axp.h
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _AXP_H_

#include <common.h>

#define pmu_err(format, arg...)	printf("[pmu]: "format, ##arg)
#define pmu_info(format, arg...)	/*printf("[pmu]: "format, ##arg)*/

typedef struct _axp_contrl_info {
	char name[8];

	u16 min_vol;
	u16 max_vol;
	u16 cfg_reg_addr;
	u16 cfg_reg_mask;

	u16 step0_val;
	u16 split1_val;
	u16 step1_val;
	u16 ctrl_reg_addr;

	u16 ctrl_bit_ofs;
	u16 res;
} axp_contrl_info;


#endif
