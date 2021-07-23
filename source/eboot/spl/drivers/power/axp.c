/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <private_boot0.h>
#include <private_toc.h>
#ifdef CFG_AXP858_POWER
#include <arch/axp858_reg.h>
#endif
#ifdef CFG_AXP81X_POWER
#include <arch/axp81x_reg.h>
#endif
#ifdef CFG_AXP809_POWER
#include <arch/axp809_reg.h>
#endif
#ifdef CFG_AXP2101_POWER
#include <arch/axp2101_reg.h>
#endif
#ifdef CFG_AXP152_POWER
#include <arch/axp152_reg.h>
#endif
#ifdef CFG_AXP1530_POWER
#include <arch/axp1530_reg.h>
#endif

extern const boot0_file_head_t fes1_head;
static u8 pmu_chip_id;

int probe_power_key(void)
{
	switch (pmu_chip_id) {
#ifdef CFG_AXP858_POWER
	case AXP858_CHIP_ID:
		return axp858_probe_power_key();
#endif
#ifdef CFG_AXP81X_POWER
	case AXP81X_CHIP_ID:
		return axp81X_probe_power_key();
#endif
#ifdef CFG_AXP809_POWER
	case AXP809_CHIP_ID:
		return axp809_probe_power_key();
#endif
#ifdef CFG_AXP2101_POWER
	case AXP2101_CHIP_ID:
		return axp2101_probe_power_key();
#endif
#ifdef CFG_AXP152_POWER
	case AXP152_CHIP_ID:
		return axp152_probe_power_key();
#endif
#ifdef CFG_AXP1530_POWER
	case AXP1530_CHIP_ID:
		return axp1530_probe_power_key();
#endif

	default:
		return -1;
	}
}

int set_ddr_voltage(int set_vol)
{
	switch (pmu_chip_id) {
#ifdef CFG_AXP858_POWER
	case AXP858_CHIP_ID:
		return axp858_set_ddr_voltage(set_vol);
#endif
#ifdef CFG_AXP81X_POWER
	case AXP81X_CHIP_ID:
		return axp81X_set_ddr_voltage(set_vol);
#endif
#ifdef CFG_AXP809_POWER
	case AXP809_CHIP_ID:
		return axp809_set_ddr_voltage(set_vol);
#endif
#ifdef CFG_AXP2101_POWER
	case AXP2101_CHIP_ID:
		return axp2101_set_ddr_voltage(set_vol);
#endif
#ifdef CFG_AXP152_POWER
	case AXP152_CHIP_ID:
		return axp152_set_ddr_voltage(set_vol);
#endif
#ifdef CFG_AXP1530_POWER
	case AXP1530_CHIP_ID:
		return axp1530_set_ddr_voltage(set_vol);
#endif

	default:
		return -1;
	}
}

int set_ddr4_2v5_voltage(int set_vol)
{
	switch (pmu_chip_id) {
#ifdef CFG_AXP858_POWER
	case AXP858_CHIP_ID:
		return axp858_set_ddr4_2v5_voltage(set_vol);
#endif
	default:
		return -1;
	}
}

int set_sys_voltage(int set_vol)
{
	switch (pmu_chip_id) {
#ifdef CFG_AXP809_POWER
	case AXP809_CHIP_ID:
		return axp809_set_sys_voltage(set_vol, 1);
#endif
#ifdef CFG_AXP2101_POWER
	case AXP2101_CHIP_ID:
		return axp2101_set_sys_voltage(set_vol, 1);
#endif
#ifdef CFG_AXP152_POWER
	case AXP152_CHIP_ID:
		return axp152_set_sys_voltage(set_vol, 1);
#endif
#ifdef CFG_AXP1530_POWER
	case AXP1530_CHIP_ID:
		return axp1530_set_sys_voltage(set_vol, 1);
#endif

	default:
		return -1;
	}
}


int set_pll_voltage(int set_vol)
{
	switch (pmu_chip_id) {
#ifdef CFG_AXP858_POWER
	case AXP858_CHIP_ID:
		return axp858_set_pll_voltage(set_vol);
#endif
#ifdef CFG_AXP81X_POWER
	case AXP81X_CHIP_ID:
		return axp81X_set_pll_voltage(set_vol);
#endif
#ifdef CFG_AXP809_POWER
	case AXP809_CHIP_ID:
		return axp809_set_pll_voltage(set_vol);
#endif
#ifdef CFG_AXP2101_POWER
	case AXP2101_CHIP_ID:
		return axp2101_set_pll_voltage(set_vol);
#endif
#ifdef CFG_AXP152_POWER
	case AXP152_CHIP_ID:
		return axp152_set_pll_voltage(set_vol);
#endif
#ifdef CFG_AXP1530_POWER
	case AXP1530_CHIP_ID:
		return axp1530_set_pll_voltage(set_vol);
#endif

	default:
		return -1;
	}
}

int set_efuse_voltage(int set_vol)
{
	switch (pmu_chip_id) {
#ifdef CFG_AXP809_POWER
	case AXP809_CHIP_ID:
		return axp809_set_efuse_voltage(set_vol);
#endif
#ifdef CFG_AXP2101_POWER
	case AXP2101_CHIP_ID:
		return axp2101_set_efuse_voltage(set_vol);
#endif
#ifdef CFG_AXP152_POWER
	case AXP152_CHIP_ID:
		return axp152_set_efuse_voltage(set_vol);
#endif
#ifdef CFG_AXP1530_POWER
	case AXP1530_CHIP_ID:
		return axp1530_set_efuse_voltage(set_vol);
#endif

	default:
		return -1;
	}
}

int axp_init(u8 power_mode)
{
#ifdef CFG_AXP858_POWER
	if (axp858_axp_init(power_mode) == AXP858_CHIP_ID) {
		return (pmu_chip_id = AXP858_CHIP_ID);
	} else
#endif
#ifdef CFG_AXP81X_POWER
		if (axp81X_axp_init(power_mode) == AXP81X_CHIP_ID) {
		return (pmu_chip_id = AXP81X_CHIP_ID);
	} else
#endif
#ifdef CFG_AXP809_POWER
		if (axp809_axp_init(power_mode) == AXP809_CHIP_ID) {
		return (pmu_chip_id = AXP809_CHIP_ID);
	} else
#endif
#ifdef CFG_AXP2101_POWER
		if (axp2101_axp_init(power_mode) == AXP2101_CHIP_ID) {
		return (pmu_chip_id = AXP2101_CHIP_ID);
	} else
#endif
#ifdef CFG_AXP152_POWER
		if (axp152_axp_init(power_mode) == AXP152_CHIP_ID) {
		return (pmu_chip_id = AXP152_CHIP_ID);
	} else
#endif
#ifdef CFG_AXP152_POWER
		if (axp1530_axp_init(power_mode) == AXP1530_CHIP_ID) {
		return (pmu_chip_id = AXP1530_CHIP_ID);
	} else
#endif

		return -1;
}

int get_power_mode(void)
{
	#ifdef CFG_SUNXI_FES
		return fes1_head.prvt_head.power_mode;
	#elif CFG_SUNXI_SBOOT
		return toc0_config->power_mode;
	#else
		return BT0_head.prvt_head.power_mode;
	#endif
}
