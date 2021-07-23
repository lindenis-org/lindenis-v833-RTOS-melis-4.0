/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <arch/pmic_bus.h>
#include <arch/axp2101_reg.h>
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

	{ "dcdc2", 500, 1540, AXP2101_DC2OUT_VOL, 0x7f, 10, 1200, 20,
	  AXP2101_OUTPUT_CTL0, 1 },
	{ "dcdc3", 500, 3400, AXP2101_DC3OUT_VOL, 0x7f, 10, 1200, 20,
	  AXP2101_OUTPUT_CTL0, 2, 100, 1540 },
	{ "dcdc4", 500, 1840, AXP2101_DC4OUT_VOL, 0x7f, 10, 1200, 20,
	  AXP2101_OUTPUT_CTL0, 3 },
	{ "bldo1", 500, 3500, AXP2101_BLDO1OUT_VOL, 0x7f, 100, 0, 0,
	  AXP2101_OUTPUT_CTL2, 4 },

};
#define PMU_POWER_KEY_STATUS AXP2101_INTSTS1
#define PMU_POWER_KEY_OFFSET 0x2

static inline void disable_dcdc_pfm_mode(void)
{
	u8 val;

	pmic_bus_read(AXP2101_RUNTIME_ADDR, 0x80, &val);
	val |= (0x01 << 3); /*dcdc4 for gpu pwm mode*/
	val |= (0x01 << 4); /*dcdc5 for dram pwm mode*/
	pmic_bus_write(AXP2101_RUNTIME_ADDR, 0x80, val);

	/* disable dcm mode for GPU stability Vdrop issue*/
	pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xff, 0x0);
	pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xf4, 0x6);
	pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xf2, 0x4);
	pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xf5, 0x4);
	pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xff, 0x1);
	pmic_bus_write(AXP2101_RUNTIME_ADDR, 0x12, 0x40);
	pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xff, 0x0);
}

int axp2101_probe_power_key(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP2101_RUNTIME_ADDR, PMU_POWER_KEY_STATUS,
			  &reg_value)) {
		return -1;
	}
	/* POKLIRQ,POKSIRQ */
	reg_value &= (0x03 << PMU_POWER_KEY_OFFSET);
	if (reg_value) {
		if (pmic_bus_write(AXP2101_RUNTIME_ADDR, PMU_POWER_KEY_STATUS,
				   reg_value)) {
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
	if (pmic_bus_read(AXP2101_RUNTIME_ADDR, p_item->cfg_reg_addr,
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

	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, p_item->cfg_reg_addr,
			   reg_value)) {
		pmu_err("unable to set %s\n", name);
		return -1;
	}
	return 0;
}

int axp2101_set_ddr_voltage(int set_vol)
{
	return pmu_set_vol("dcdc4", set_vol, 1);
}

int axp2101_set_efuse_voltage(int set_vol)
{
	return pmu_set_vol("bldo1", set_vol, 1);
}

int axp2101_set_pll_voltage(int set_vol)
{
	return pmu_set_vol("dcdc2", set_vol, 1);
}

int axp2101_set_sys_voltage(int set_vol, int onoff)
{
	return pmu_set_vol("dcdc3", set_vol, onoff);
}

s32 set_chgcur_limit(void)
{
	/* limit charge current to 300mA */
	return pmic_bus_write(AXP2101_RUNTIME_ADDR, AXP2101_CHARGE1, 0x9);
}

s32 enable_vbus_adc_channel(void)
{
	return pmic_bus_write(AXP2101_RUNTIME_ADDR, AXP2101_BAT_AVERVOL_H6, 0x40);
}

s32 set_dcdc1_pwm_mode(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP2101_RUNTIME_ADDR, AXP2101_OUTPUT_CTL1, &reg_value))
		return -1;
	reg_value |= (1 << 2);
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, AXP2101_BAT_AVERVOL_H6, reg_value))
		return -1;
	return 0;
}

s32 pmu_disable_soften3_signal(void)
{
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xFF, 0x00))
		return -1;
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xF0, 0x06))
		return -1;
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xF1, 0x04))
		return -1;
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xFF, 0x01))
		return -1;
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, 0X26, 0x30))
		return -1;
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xFF, 0x00))
		return -1;
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xF1, 0x00))
		return -1;
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, 0xF0, 0x00))
		return -1;
	return 0;
}

int pmu_set_vsys_min(void)
{
	u8 reg;
	pmic_bus_read(AXP2101_RUNTIME_ADDR, AXP2101_VSYS_MIN, &reg);
	reg &= ~(0x7 << 4);
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, AXP2101_VSYS_MIN, reg)) {
		pmu_err("set vsys_min error\n");
		return -1;
	}
	return 0;
}

int pmu_set_vimdpm_cfg(void)
{
	u8 reg;
	pmic_bus_read(AXP2101_RUNTIME_ADDR, AXP2101_VBUS_VOL_SET, &reg);
	reg &= ~(0xf << 0);
	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, AXP2101_VBUS_VOL_SET, reg)) {
		pmu_err("set vimdpm error\n");
		return -1;
	}
	return 0;
}


s32 pmu_reset_enable(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP2101_RUNTIME_ADDR, AXP2101_OFF_CTL, &reg_value))
		return -1;
	reg_value |= (1 << 3);

	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, AXP2101_OFF_CTL, reg_value))
		return -1;
	return 0;
}

u32 pmu_pwroff_enable(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP2101_RUNTIME_ADDR, AXP2101_PWEON_PWEOFF_EN, &reg_value))
		return -1;
	reg_value |= (1 << 1);

	if (pmic_bus_write(AXP2101_RUNTIME_ADDR, AXP2101_PWEON_PWEOFF_EN, reg_value))
		return -1;
	return 0;
}

int axp2101_axp_init(u8 power_mode)
{
	u8 pmu_type;

	if (pmic_bus_init(AXP2101_DEVICE_ADDR, AXP2101_RUNTIME_ADDR)) {
		pmu_err("bus init error\n");
		return -1;
	}

	if (pmic_bus_read(AXP2101_RUNTIME_ADDR, AXP2101_VERSION, &pmu_type)) {
		pmu_err("bus read error\n");
		return -1;
	}

	pmu_type &= 0xCF;
	if (pmu_type == 0x47) {
		/* pmu type AXP21 */
		printf("PMU: AXP21\n");
		set_chgcur_limit();
		enable_vbus_adc_channel();
		set_dcdc1_pwm_mode();
		pmu_disable_soften3_signal();
		pmu_set_vsys_min();
		pmu_set_vimdpm_cfg();
		pmu_reset_enable();
		pmu_pwroff_enable();
		axp2101_set_sys_voltage(VDD_SYS_VOL, VOL_ON);
		return AXP2101_CHIP_ID;
	}
	printf("unknow PMU\n");
	return -1;
}
