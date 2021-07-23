/*
 * disp_board_config.c
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
#include <sunxi_hal_gpio.h>
#include "../disp/disp_sys_intf.h"
#include "disp_board_config.h"

struct property_t g_lcd0_config[] = {
	{
		.name = "lcd_used",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_driver_name",
		.type = PROPERTY_STRING,
		.v.str = "lx50hdi4002",
	},
	{
		.name = "lcd_backlight",
		.type = PROPERTY_INTGER,
		.v.value = 150,
	},
	{
		.name = "lcd_if",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
	{
		.name = "lcd_x",
		.type = PROPERTY_INTGER,
		.v.value = 720,
	},
	{
		.name = "lcd_y",
		.type = PROPERTY_INTGER,
		.v.value = 1280,
	},
	{
		.name = "lcd_width",
		.type = PROPERTY_INTGER,
		.v.value = 65,
	},
	{
		.name = "lcd_height",
		.type = PROPERTY_INTGER,
		.v.value = 118,
	},
	{
		.name = "lcd_dclk_freq",
		.type = PROPERTY_INTGER,
		.v.value = 56,
	},
	{
		.name = "lcd_pwm_used",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_pwm_ch",
		.type = PROPERTY_INTGER,
		.v.value = 9,
	},
	{
		.name = "lcd_pwm_freq",
		.type = PROPERTY_INTGER,
		.v.value = 50000,
	},
	{
		.name = "lcd_pwm_pol",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_pwm_max_limit",
		.type = PROPERTY_INTGER,
		.v.value = 255,
	},
	{
		.name = "lcd_hbp",
		.type = PROPERTY_INTGER,
		.v.value = 20,
	},
	{
		.name = "lcd_ht",
		.type = PROPERTY_INTGER,
		.v.value = 768,
	},
	{
		.name = "lcd_hspw",
		.type = PROPERTY_INTGER,
		.v.value = 8,
	},
	{
		.name = "lcd_vbp",
		.type = PROPERTY_INTGER,
		.v.value = 16,
	},
	{
		.name = "lcd_vt",
		.type = PROPERTY_INTGER,
		.v.value = 1314,
	},
	{
		.name = "lcd_vspw",
		.type = PROPERTY_INTGER,
		.v.value = 2,
	},
	{
		.name = "lcd_dsi_if",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_dsi_lane",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
	{
		.name = "lcd_dsi_format",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_dsi_te",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_frm",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_power",
		.type = PROPERTY_POWER,
		.v.power = {
			.power_name = "dcdc1",
			.power_type = AXP2101_REGULATOR,
			.power_id = AXP2101_ID_DCDC1,
			.power_vol = 3300000,
		},
	},
	{
		.name = "lcd_power1",
		.type = PROPERTY_POWER,
		.v.power = {
			.power_name = "aldo4",
			.power_type = AXP2101_REGULATOR,
			.power_id = AXP2101_ID_ALDO4,
			.power_vol = 1800000,
		},
	},
	{
		.name = "lcd_pin_power",
		.type = PROPERTY_POWER,
		.v.power = {
			.power_name = "bldo1",
			.power_type = AXP2101_REGULATOR,
			.power_id = AXP2101_ID_BLDO1,
			.power_vol = 1800000,
			.always_on = true,
		},
	},
	{
		.name = "lcd_gpio_0",
		.type = PROPERTY_GPIO,
		.v.gpio_list = {
			.gpio = GPIOD(9),
			.mul_sel = GPIO_DIRECTION_OUTPUT,
			.pull = 0,
			.drv_level = 3,
			.data = 1,
		},
	},

};

struct property_t g_lcd1_config[] = {
	{
		.name = "lcd_used",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
};

struct property_t g_disp_config[] = {
	{
		.name = "disp_init_enable",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "disp_mode",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "screen0_output_type",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "screen0_output_mode",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
	{
		.name = "screen1_output_type",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "screen1_output_mode",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
};

u32 g_lcd0_config_len = sizeof(g_lcd0_config) / sizeof(struct property_t);
u32 g_lcd1_config_len = sizeof(g_lcd1_config) / sizeof(struct property_t);
u32 g_disp_config_len = sizeof(g_disp_config) / sizeof(struct property_t);
