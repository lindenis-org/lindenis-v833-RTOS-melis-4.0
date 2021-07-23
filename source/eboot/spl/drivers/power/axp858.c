/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <arch/pmic_bus.h>
#include <arch/axp858_reg.h>

#define pmu_err(format, arg...) printf("[pmu]: " format, ##arg)
#define pmu_info(format, arg...) /*printf("[pmu]: "format,##arg)*/

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

static int pmu_set_vol(char *name, int set_vol, int onoff);

static axp_contrl_info axp_ctrl_tbl[] = {
	/*   name        min,     max,    reg,    mask,  step0,   split1_val,  step1,   ctrl_reg,           ctrl_bit */

	{ "dcdc2", 500, 1540, 0x15, 0x7f, 10, 1200, 20, PMU_DC3OUT_VOL, 3 },
	{ "dcdc5", 800, 1840, 0x24, 0x7f, 10, 1120, 20, PMU_DC5OUT_VOL, 5 },
	{ "aldo2", 700, 3300, 0x20, 0x1f, 100, 0, 0, PMU_ALDO2_VOL, 1 },
	{ "aldo5", 700, 3300, 0x23, 0x1f, 100, 0, 0, PMU_ALDO5_VOL, 4 },

};
#define PMU_POWER_KEY_STATUS 0x4c
#define PMU_POWER_KEY_OFFSET 0x3
#if 0
static int pwrok_restart_enable(void)
{
	u8 reg_val = 0;
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, PMU_POWER_DISABLE_DOWN, &reg_val)) {
		return -1;
	}
	/* PWROK drive low restart function enable  */
	/* for watchdog reset */
	reg_val |= (0x1U << 6);
	if (pmic_bus_write(AXP858_RUNTIME_ADDR, PMU_POWER_DISABLE_DOWN, reg_val)) {
		return -1;
	}
	return 0;
}
#endif
static inline void disable_dcdc_pfm_mode(void)
{
	u8 val;

	pmic_bus_read(AXP858_RUNTIME_ADDR, 0x80, &val);
	val |= (0x01 << 3); /*dcdc4 for gpu pwm mode*/
	val |= (0x01 << 4); /*dcdc5 for dram pwm mode*/
	pmic_bus_write(AXP858_RUNTIME_ADDR, 0x80, val);

	/* disable dcm mode for GPU stability Vdrop issue*/
	pmic_bus_write(AXP858_RUNTIME_ADDR, 0xff, 0x0);
	pmic_bus_write(AXP858_RUNTIME_ADDR, 0xf4, 0x6);
	pmic_bus_write(AXP858_RUNTIME_ADDR, 0xf2, 0x4);
	pmic_bus_write(AXP858_RUNTIME_ADDR, 0xf5, 0x4);
	pmic_bus_write(AXP858_RUNTIME_ADDR, 0xff, 0x1);
	pmic_bus_write(AXP858_RUNTIME_ADDR, 0x12, 0x40);
	pmic_bus_write(AXP858_RUNTIME_ADDR, 0xff, 0x0);
}

int axp858_probe_power_key(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP858_RUNTIME_ADDR, PMU_POWER_KEY_STATUS, &reg_value)) {
		return -1;
	}
	/* POKLIRQ,POKSIRQ */
	reg_value &= (0x03 << PMU_POWER_KEY_OFFSET);
	if (reg_value) {
		if (pmic_bus_write(AXP858_RUNTIME_ADDR, PMU_POWER_KEY_STATUS, reg_value)) {
			return -1;
		}
	}

	return (reg_value >> PMU_POWER_KEY_OFFSET) & 3;
}

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
	return (axp_ctrl_tbl + i);
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
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, p_item->cfg_reg_addr, &reg_value)) {
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

	if (pmic_bus_write(AXP858_RUNTIME_ADDR, p_item->cfg_reg_addr, reg_value)) {
		pmu_err("unable to set %s\n", name);
		return -1;
	}
	return 0;
}

int axp858_set_ddr_voltage(int set_vol)
{
	return pmu_set_vol("dcdc5", set_vol, 1);
}

int axp858_set_ddr4_2v5_voltage(int set_vol)
{
	return pmu_set_vol("aldo5", set_vol, 1);
}

int axp858_set_pll_voltage(int set_vol)
{
	return pmu_set_vol("dcdc2", set_vol, 1);
}

int axp858_axp_init(u8 power_mode)
{
	u8 pmu_type;

	if (pmic_bus_init(AXP858_DEVICE_ADDR, AXP858_RUNTIME_ADDR)) {
		pmu_err("bus init error\n");
		return -1;
	}

	if (pmic_bus_read(AXP858_RUNTIME_ADDR, PMU_IC_TYPE, &pmu_type)) {
		pmu_err("bus read error\n");
		return -1;
	}

	pmu_type &= 0xCF;
	if (pmu_type == 0x44) {
		/* pmu type AXP858 */
		printf("PMU: AXP858\n");
		/*pwrok_restart_enable();*/
		/*disable_dcdc_pfm_mode();*/
		return AXP858_CHIP_ID;
	}
	printf("unknow PMU\n");
	return -1;
}
