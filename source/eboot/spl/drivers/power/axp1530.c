/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <arch/pmic_bus.h>
#include <arch/axp1530_reg.h>
#include <private_boot0.h>

#define pmu_err(format, arg...) printf("[pmu]: " format, ##arg)
#define pmu_info(format, arg...) /*printf("[pmu]: "format,##arg)*/

#define VDD_SYS_VOL (920)
#define VOL_ON (1)

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
	u16 step2_val;
	u16 split2_val;
} axp_contrl_info;

static int pmu_set_vol(char *name, int set_vol, int onoff);

static axp_contrl_info axp_ctrl_tbl[] = {
	/*   name        min,     max,    reg,    mask,  step0,   split1_val,  step1,   ctrl_reg,           ctrl_bit */
	{ "dcdc1", 500, 3400, AXP1530_DC1OUT_VOL, 0x7f, 10, 1200, 20,
	  AXP1530_OUTPUT_POWER_ON_OFF_CTL, 0, 100, 1540 },
	{ "dcdc2", 500, 1540, AXP1530_DC2OUT_VOL, 0x7f, 10, 1200, 20,
	  AXP1530_OUTPUT_POWER_ON_OFF_CTL, 1 },
	{ "dcdc3", 500, 1840, AXP1530_DC3OUT_VOL, 0x7f, 10, 1200, 20,
	  AXP1530_OUTPUT_POWER_ON_OFF_CTL, 2 },
	{ "aldo1", 500, 3500, AXP1530_ALDO1OUT_VOL, 0x1f, 100, 0, 0,
	  AXP1530_OUTPUT_POWER_ON_OFF_CTL, 3 },
	{ "dldo1", 500, 3500, AXP1530_DLDO1OUT_VOL, 0x1f, 100, 0, 0,
	  AXP1530_OUTPUT_POWER_ON_OFF_CTL, 4 },
};


int axp1530_probe_power_key(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP1530_RUNTIME_ADDR, AXP1530_IRQ_STATUS, &reg_value)) {
		return -1;
	}
	reg_value &= (0x03 << 4);
	if (reg_value) {
		if (pmic_bus_write(AXP1530_RUNTIME_ADDR, AXP1530_IRQ_STATUS,
				   reg_value)) {
			return -1;
		}
	}
	return (reg_value >> 4) & 3;

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
	if (pmic_bus_read(AXP1530_RUNTIME_ADDR, p_item->cfg_reg_addr,
			  &reg_value)) {
		return -1;
	}

	reg_value &= ~p_item->cfg_reg_mask;
	if (p_item->split2_val && (set_vol > p_item->split2_val)) {
		base_step = (p_item->split2_val - p_item->split1_val) /
			    p_item->step1_val;

		base_step += (p_item->split1_val - p_item->min_vol) /
			     p_item->step0_val;
		reg_value |=
			(base_step +
			 (set_vol - p_item->split2_val) / p_item->step2_val);
	} else if (p_item->split1_val && (set_vol > p_item->split1_val)) {
		if (p_item->split1_val < p_item->min_vol) {
			pmu_err("bad split val(%d) for %s\n",
				p_item->split1_val, name);
		}

		base_step = (p_item->split1_val - p_item->min_vol) /
			    p_item->step0_val;
		reg_value |=
			(base_step +
			 (set_vol - p_item->split1_val) / p_item->step1_val);
	} else {
		reg_value |= (set_vol - p_item->min_vol) / p_item->step0_val;
	}

	if (pmic_bus_write(AXP1530_RUNTIME_ADDR, p_item->cfg_reg_addr,
			   reg_value)) {
		pmu_err("unable to set %s\n", name);
		return -1;
	}
	return 0;
}

int axp1530_set_ddr_voltage(int set_vol)
{
	return pmu_set_vol("dcdc3", set_vol, 1);
}

int axp1530_set_efuse_voltage(int set_vol)
{
	return pmu_set_vol("aldo1", set_vol, 1);
}

int axp1530_set_pll_voltage(int set_vol)
{
	return pmu_set_vol("dcdc2", set_vol, 1);
}

int axp1530_set_sys_voltage(int set_vol, int onoff)
{
	return pmu_set_vol("dcdc1", set_vol, onoff);
}

static int axp1530_necessary_reg_enable(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP1530_RUNTIME_ADDR, AXP1530_WRITE_LOCK, &reg_value))
		return -1;
	reg_value |= 0x5;
	if (pmic_bus_write(AXP1530_RUNTIME_ADDR, AXP1530_WRITE_LOCK, reg_value))
		return -1;

	if (pmic_bus_read(AXP1530_RUNTIME_ADDR, AXP1530_ERROR_MANAGEMENT, &reg_value))
		return -1;
	reg_value |= 0x8;
	if (pmic_bus_write(AXP1530_RUNTIME_ADDR, AXP1530_ERROR_MANAGEMENT, reg_value))
		return -1;

	if (pmic_bus_read(AXP1530_RUNTIME_ADDR, AXP1530_DCDC_DVM_PWM_CTL, &reg_value))
		return -1;
	reg_value |= (0x1 << 5);
	if (pmic_bus_write(AXP1530_RUNTIME_ADDR, AXP1530_DCDC_DVM_PWM_CTL, reg_value))
		return -1;
	return 0;
}


int axp1530_axp_init(u8 power_mode)
{
	u8 pmu_type;

	if (pmic_bus_init(AXP1530_DEVICE_ADDR, AXP1530_RUNTIME_ADDR)) {
		pmu_err("bus init error\n");
		return -1;
	}

	if (pmic_bus_read(AXP1530_RUNTIME_ADDR, AXP1530_VERSION, &pmu_type)) {
		pmu_err("bus read error\n");
		return -1;
	}

	pmu_type &= 0xCF;
	if (pmu_type == AXP1530_CHIP_ID) {
		/* pmu type AXP1530 */
		axp1530_necessary_reg_enable();
		printf("PMU: AXP1530\n");
		return AXP1530_CHIP_ID;
	}
	printf("unknow PMU\n");
	return -1;
}

