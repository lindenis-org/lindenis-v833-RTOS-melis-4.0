/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 * axp809
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <arch/pmic_bus.h>
#include <arch/axp809_reg.h>
#include "axp.h"

#define PMU_IC_TYPY_REG (0x3)

#define VDD_SYS_VOL (920)
#define VOL_ON (1)

static axp_contrl_info axp_ctrl_tbl[] = {
	/*   name     min,  max,   reg,   mask,  step0,
split1_val,  step1,   ctrl_reg,     ctrl_bit */

	{ "dcdc2", 600, 1540, 0x22, 0x7f, 20, 0, 0, BOOT_POWER809_OUTPUT_CTL1,
	  2 },

	{ "dcdc3", 600, 1860, 0x23, 0x7f, 20, 0, 0, BOOT_POWER809_OUTPUT_CTL1,
	  3 },

	{ "dcdc5", 1000, 2550, 0x25, 0x7f, 50, 0, 0, BOOT_POWER809_OUTPUT_CTL1,
	  5 },

	{ "aldo3", 700, 3300, 0x2a, 0x7f, 100, 0, 0, BOOT_POWER809_OUTPUT_CTL2,
	  5 },
};
#define PMU_POWER_KEY_STATUS BOOT_POWER809_INTSTS5
#define PMU_POWER_KEY_OFFSET 0x3

static axp_contrl_info *get_ctrl_info_from_tbl(char *name)
{
	int i    = 0;
	int size = ARRAY_SIZE(axp_ctrl_tbl);
	for (i = 0; i < size; i++) {
		if (!strncmp(name, axp_ctrl_tbl[i].name,
			     strlen(axp_ctrl_tbl[i].name))) {
			break;
		}
	}
	if (i >= size) {
		return NULL;
	}
	return axp_ctrl_tbl + i;
}

static int pmu_set_vol(char *name, int set_vol, int onoff)
{
	u8 reg_value;
	axp_contrl_info *p_item = NULL;
	u8 base_step;

	if (set_vol <= 0)
		return 0;

	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}

	pmu_info(
		"name %s, min_vol %dmv, max_vol %d, cfg_reg 0x%x, cfg_mask 0x%x \
		step0_val %d, split1_val %d, step1_val %d, ctrl_reg_addr 0x%x, ctrl_bit_ofs %d\n",
		p_item->name, p_item->min_vol, p_item->max_vol,
		p_item->cfg_reg_addr, p_item->cfg_reg_mask, p_item->step0_val,
		p_item->split1_val, p_item->step1_val, p_item->ctrl_reg_addr,
		p_item->ctrl_bit_ofs);

	if (set_vol < p_item->min_vol) {
		set_vol = p_item->min_vol;
	} else if (set_vol > p_item->max_vol) {
		set_vol = p_item->max_vol;
	}
	if (pmic_bus_read(AXP809_RUNTIME_ADDR, p_item->cfg_reg_addr,
			  &reg_value)) {
		return -1;
	}

	reg_value &= ~p_item->cfg_reg_mask;
	if (p_item->split1_val && (set_vol > p_item->split1_val)) {
		if (p_item->split1_val < p_item->min_vol) {
			pmu_err("bad split val(%d) for %s\n",
				p_item->split1_val, name);
			return -1;
		}

		base_step = (p_item->split1_val - p_item->min_vol) /
			    p_item->step0_val;
		reg_value |=
			(base_step +
			 (set_vol - p_item->split1_val) / p_item->step1_val);
	} else {
		reg_value |= (set_vol - p_item->min_vol) / p_item->step0_val;
	}

	if (pmic_bus_write(AXP809_RUNTIME_ADDR, p_item->cfg_reg_addr,
			   reg_value)) {
		pmu_err("unable to set %s\n", name);
		return -1;
	}

#if 0
	if (onoff < 0) {
		return 0;
	}
	if (pmic_bus_read(AXP809_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			  &reg_value)) {
		return -1;
	}
	if (onoff == 0) {
		reg_value &= ~(1 << p_item->ctrl_bit_ofs);
	} else {
		reg_value |= (1 << p_item->ctrl_bit_ofs);
	}
	if (pmic_bus_write(AXP809_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			   reg_value)) {
		pmu_err("unable to onoff %s\n", name);
		return -1;
	}
#endif
	return 0;
}

int axp809_probe_power_key(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP809_RUNTIME_ADDR, PMU_POWER_KEY_STATUS,
			  &reg_value)) {
		return -1;
	}
	/* POKLIRQ,POKSIRQ */
	reg_value &= (0x03 << PMU_POWER_KEY_OFFSET);
	if (reg_value) {
		if (pmic_bus_write(AXP809_RUNTIME_ADDR, PMU_POWER_KEY_STATUS,
				   reg_value)) {
			return -1;
		}
	}

	return (reg_value >> PMU_POWER_KEY_OFFSET) & 3;
}

int axp809_set_ddr_voltage(int set_vol)
{
	return pmu_set_vol("dcdc5", set_vol, 1);
}

int axp809_set_pll_voltage(int set_vol)
{
	return pmu_set_vol("dcdc2", set_vol, 1);
}

int axp809_set_efuse_voltage(int set_vol)
{
	return pmu_set_vol("aldo3", set_vol, 1);
}

int axp809_set_sys_voltage(int set_vol, int onoff)
{
	return pmu_set_vol("dcdc3", set_vol, onoff);
}

int axp809_axp_init(u8 power_mode)
{
	u8 pmu_type;

	if (pmic_bus_init(AXP809_DEVICE_ADDR, AXP809_RUNTIME_ADDR)) {
		pmu_err("bus init error\n");
		return -1;
	}

	if (pmic_bus_read(AXP809_RUNTIME_ADDR, PMU_IC_TYPY_REG, &pmu_type)) {
		pmu_err("bus read error\n");
		return -1;
	}

	pmu_type &= 0xCF;
	if (pmu_type == 0x42) {
		/* pmu type AXP809 */
		printf("PMU: AXP809\n");
		axp809_set_sys_voltage(VDD_SYS_VOL, VOL_ON);
		return AXP809_CHIP_ID;
	}
	printf("unknow PMU\n");
	return -1;
}
