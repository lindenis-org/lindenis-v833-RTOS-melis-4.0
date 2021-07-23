/*
 * drivers/input/sensor/sunxi_gpadc.h
 *
 * Copyright (C) 2016 Allwinner.
 * fuzhaoke <fuzhaoke@allwinnertech.com>
 *
 * SUNXI GPADC Controller Driver Header
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef HAL_GPADC_H
#define HAL_GPADC_H

#include "hal_clk.h"
#include "sunxi_hal_common.h"
#include <log.h>
#include <interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_DRIVERS_GPADC_DEBUG
#ifdef CONFIG_DRIVERS_GPADC_DEBUG
#define GPADC_INFO(fmt, arg...) __log(fmt, ##arg)
#else
#define GPADC_INFO(fmt, arg...) do {}while(0)
#endif

#define GPADC_ERR(fmt, arg...) __err(fmt, ##arg)

#define GPADC_BASE 0x05070000
#define SUNXI_GPADC_IRQ 32

#define OSC_24MHZ       (24000000UL)
#define MAX_SR          (100000UL)
#define MIN_SR          (400UL)
#define DEFAULT_SR      (1000UL)
/* voltage range 0~2.3v, unit is uv */
#define VOL_RANGE       (1800000UL)
#define VOL_VALUE_MASK      (0Xfff)
#define COMPARE_LOWDATA   1700000UL
#define COMPARE_HIGDATA   1200000UL

/* GPADC register offset */
#define GP_SR_REG       (0x00) /* Sample Rate config register */
#define GP_CTRL_REG     (0x04) /* control register */
#define GP_CS_EN_REG        (0x08) /* compare and select enable register */
#define GP_FIFO_INTC_REG    (0x0c) /* FIFO interrupt config register */
#define GP_FIFO_INTS_REG    (0x10) /* FIFO interrupt status register */
#define GP_FIFO_DATA_REG    (0X14) /* FIFO data register */
#define GP_CB_DATA_REG      (0X18) /* calibration data register */
#define GP_DATAL_INTC_REG   (0x20)
#define GP_DATAH_INTC_REG   (0x24)
#define GP_DATA_INTC_REG    (0x28)
#define GP_DATAL_INTS_REG   (0x30)
#define GP_DATAH_INTS_REG   (0x34)
#define GP_DATA_INTS_REG    (0x38)
#define GP_CH0_CMP_DATA_REG (0x40) /* channal 0 compare data register */
#define GP_CH1_CMP_DATA_REG (0x44) /* channal 1 compare data register */
#define GP_CH2_CMP_DATA_REG (0x48) /* channal 2 compare data register */
#define GP_CH3_CMP_DATA_REG (0x4c) /* channal 3 compare data register */
#define GP_CH4_CMP_DATA_REG (0x50) /* channal 4 compare data register */
#define GP_CH5_CMP_DATA_REG (0x54) /* channal 5 compare data register */
#define GP_CH6_CMP_DATA_REG (0x58) /* channal 6 compare data register */
#define GP_CH7_CMP_DATA_REG (0x5c) /* channal 7 compare data register */
#define GP_CH0_DATA_REG     (0x80) /* channal 0 data register */
#define GP_CH1_DATA_REG     (0x84) /* channal 1 data register */
#define GP_CH2_DATA_REG     (0x88) /* channal 2 data register */
#define GP_CH3_DATA_REG     (0x8c) /* channal 3 data register */
#define GP_CH4_DATA_REG     (0x90) /* channal 4 data register */
#define GP_CH5_DATA_REG     (0x94) /* channal 5 data register */
#define GP_CH6_DATA_REG     (0x98) /* channal 6 data register */
#define GP_CH7_DATA_REG     (0x9c) /* channal 7 data register */

/*
 * GP_SR_REG default value: 0x01df_002f 50KHZ
 * sample_rate = clk_in/(n+1) = 24MHZ/(0x1df + 1) = 50KHZ
 */
#define GP_SR_CON       (0xffff << 16)

/* GP_CTRL_REG default value:0x0000_0000 */
#define GP_FIRST_CONCERT_DLY    (0xff<<24) /* delay time of the first time */
#define GP_CALI_EN      (1 << 17) /* enable calibration */
#define GP_ADC_EN       (1 << 16) /* GPADC function enable */

/*
 * 00:single conversion mode
 * 01:single-cycle conversion mode
 * 10:continuous mode, 11:burst mode
 */
#define GP_MODE_SELECT      (3 << 8)

/* 0:disable, 1:enable */
#define GP_CH7_CMP_EN       (1 << 23)
#define GP_CH6_CMP_EN       (1 << 22)
#define GP_CH5_CMP_EN       (1 << 21)
#define GP_CH4_CMP_EN       (1 << 20)
#define GP_CH3_CMP_EN       (1 << 19)
#define GP_CH2_CMP_EN       (1 << 18)
#define GP_CH1_CMP_EN       (1 << 17)
#define GP_CH0_CMP_EN       (1 << 16)
#define GP_CH7_SELECT       (1 << 7)
#define GP_CH6_SELECT       (1 << 6)
#define GP_CH5_SELECT       (1 << 5)
#define GP_CH4_SELECT       (1 << 4)
#define GP_CH3_SELECT       (1 << 3)
#define GP_CH2_SELECT       (1 << 2)
#define GP_CH1_SELECT       (1 << 1)
#define GP_CH0_SELECT       (1 << 0)

/*
 * GP_FIFO_INTC_REG default value: 0x0000_0f00
 * 0:disable, 1:enable
 */
#define FIFO_OVER_IRQ_EN    (1 << 17) /* fifo over run irq enable */
#define FIFO_DATA_IRQ_EN    (1 << 16) /* fifo data irq enable */

/* write 1 to flush TX FIFO, self clear to 0 */
#define FIFO_FLUSH      (1 << 4)

/*
 * GP_FIFO_INTS_REG default value: 0x0000_0000
 * 0:no pending irq, 1: over pending, need write 1 to clear flag
 */
#define FIFO_OVER_PEND      (1 << 17) /* fifo over pending flag */
#define FIFO_DATA_PEND      (1 << 16) /* fifo data pending flag */
#define FIFO_CNT        (0x3f << 8) /* the data count in fifo */

/* GP_FIFO_DATA_REG default value: 0x0000_0000 */
#define GP_FIFO_DATA        (0xfff << 0) /* GPADC data in fifo */

/* GP_CB_DATA_REG default value: 0x0000_0000 */
#define GP_CB_DATA      (0xfff << 0) /* GPADC calibration data */

/* GP_INTC_REG default value: 0x0000_0000 */
#define GP_CH7_LOW_IRQ_EN   (1 << 7) /* 0:disable, 1:enable */
#define GP_CH6_LOW_IRQ_EN   (1 << 6)
#define GP_CH5_LOW_IRQ_EN   (1 << 5)
#define GP_CH4_LOW_IRQ_EN   (1 << 4)
#define GP_CH3_LOW_IRQ_EN   (1 << 3)
#define GP_CH2_LOW_IRQ_EN   (1 << 2)
#define GP_CH1_LOW_IRQ_EN   (1 << 1)
#define GP_CH0_LOW_IRQ_EN   (1 << 0)
#define GP_CH7_HIG_IRQ_EN   (1 << 7)
#define GP_CH6_HIG_IRQ_EN   (1 << 6)
#define GP_CH5_HIG_IRQ_EN   (1 << 5)
#define GP_CH4_HIG_IRQ_EN   (1 << 4)
#define GP_CH3_HIG_IRQ_EN   (1 << 3)
#define GP_CH2_HIG_IRQ_EN   (1 << 2)
#define GP_CH1_HIG_IRQ_EN   (1 << 1)
#define GP_CH0_HIG_IRQ_EN   (1 << 0)
#define GP_CH7_DATA_IRQ_EN  (1 << 7)
#define GP_CH6_DATA_IRQ_EN  (1 << 6)
#define GP_CH5_DATA_IRQ_EN  (1 << 5)
#define GP_CH4_DATA_IRQ_EN  (1 << 4)
#define GP_CH3_DATA_IRQ_EN  (1 << 3)
#define GP_CH2_DATA_IRQ_EN  (1 << 2)
#define GP_CH1_DATA_IRQ_EN  (1 << 1)
#define GP_CH0_DATA_IRQ_EN  (1 << 0)

/* GP_INTS_REG default value: 0x0000_0000 */
#define GP_CH7_LOW      (1 << 7) /* 0:no pending, 1:pending */
#define GP_CH6_LOW      (1 << 6)
#define GP_CH5_LOW      (1 << 5)
#define GP_CH4_LOW      (1 << 4)
#define GP_CH3_LOW      (1 << 3)
#define GP_CH2_LOW      (1 << 2)
#define GP_CH1_LOW      (1 << 1)
#define GP_CH0_LOW      (1 << 0)
#define GP_CH7_HIG      (1 << 7)
#define GP_CH6_HIG      (1 << 6)
#define GP_CH5_HIG      (1 << 5)
#define GP_CH4_HIG      (1 << 4)
#define GP_CH3_HIG      (1 << 3)
#define GP_CH2_HIG      (1 << 2)
#define GP_CH1_HIG      (1 << 1)
#define GP_CH0_HIG      (1 << 0)
#define GP_CH7_DATA     (1 << 7)
#define GP_CH6_DATA     (1 << 6)
#define GP_CH5_DATA     (1 << 5)
#define GP_CH4_DATA     (1 << 4)
#define GP_CH3_DATA     (1 << 3)
#define GP_CH2_DATA     (1 << 2)
#define GP_CH1_DATA     (1 << 1)
#define GP_CH0_DATA     (1 << 0)

/* GP_CH0_CMP_DATA_REG default value 0x0bff_0400 */
#define GP_CH0_CMP_HIG_DATA     (0xfff << 16)
#define GP_CH0_CMP_LOW_DATA     (0xfff << 0)
/* GP_CH1_CMP_DATA_REG default value 0x0bff_0400 */
#define GP_CH1_CMP_HIG_DATA     (0xfff << 16)
#define GP_CH1_CMP_LOW_DATA     (0xfff << 0)
/* GP_CH2_CMP_DATA_REG default value 0x0bff_0400 */
#define GP_CH2_CMP_HIG_DATA     (0xfff << 16)
#define GP_CH2_CMP_LOW_DATA     (0xfff << 0)
/* GP_CH3_CMP_DATA_REG default value 0x0bff_0400 */
#define GP_CH3_CMP_HIG_DATA     (0xfff << 16)
#define GP_CH3_CMP_LOW_DATA     (0xfff << 0)
/* GP_CH4_CMP_DATA_REG default value 0x0bff_0400 */
#define GP_CH4_CMP_HIG_DATA     (0xfff << 16)
#define GP_CH4_CMP_LOW_DATA     (0xfff << 0)
/* GP_CH5_CMP_DATA_REG default value 0x0bff_0400 */
#define GP_CH5_CMP_HIG_DATA     (0xfff << 16)
#define GP_CH5_CMP_LOW_DATA     (0xfff << 0)
/* GP_CH6_CMP_DATA_REG default value 0x0bff_0400 */
#define GP_CH6_CMP_HIG_DATA     (0xfff << 16)
#define GP_CH6_CMP_LOW_DATA     (0xfff << 0)
/* GP_CH7_CMP_DATA_REG default value 0x0bff_0400 */
#define GP_CH7_CMP_HIG_DATA     (0xfff << 16)
#define GP_CH7_CMP_LOW_DATA     (0xfff << 0)

/* GP_CH0_DATA_REG default value:0x0000_0000 */
#define GP_CH_DATA_MASK     (0xfff << 0) /*data mask */

#define CHANNEL_0_SELECT        (0x01 << 0)
#define CHANNEL_1_SELECT        (0x01 << 1)
#define CHANNEL_2_SELECT        (0x01 << 2)
#define CHANNEL_3_SELECT        (0x01 << 3)
#define CHANNEL_4_SELECT        (0x01 << 4)
#define CHANNEL_5_SELECT        (0x01 << 5)
#define CHANNEL_6_SELECT        (0x01 << 6)
#define CHANNEL_7_SELECT        (0x01 << 7)
#define CHANNEL_MAX_NUM         8
#define CHANNEL_NUM 4

enum
{
    GPADC_DOWN,
    GPADC_UP
};

typedef enum
{
    GP_CH_0 = 0,
    GP_CH_1,
    GP_CH_2,
    GP_CH_3,
    GP_CH_4,
    GP_CH_5,
    GP_CH_6,
    GP_CH_7,
    GP_CH_MAX
} hal_gpadc_channel_t;

typedef enum
{
    GPADC_IRQ_ERROR = -4,
    GPADC_CHANNEL_ERROR = -3,
    GPADC_CLK_ERROR = -2,
    GPADC_ERROR = -1,
    GPADC_OK = 0,
} hal_gpadc_status_t;

typedef enum gp_select_mode
{
    GP_SINGLE_MODE = 0,
    GP_SINGLE_CYCLE_MODE,
    GP_CONTINUOUS_MODE,
    GP_BURST_MODE,
} hal_gpadc_mode_t;

typedef int (*gpadc_callback_t)(uint32_t data_type, uint32_t data);

typedef struct
{
    uint32_t reg_base;
    uint32_t channel_num;
    uint32_t irq_num;
    uint32_t sample_rate;
    hal_clk_id_t mclk;
    hal_clk_id_t pclk;
    hal_gpadc_mode_t mode;
    gpadc_callback_t callback[CHANNEL_MAX_NUM];
} hal_gpadc_t;

int hal_gpadc_init(void);
hal_gpadc_status_t hal_gpadc_exit(void);
hal_gpadc_status_t hal_gpadc_channel_init(hal_gpadc_channel_t channal);
hal_gpadc_status_t hal_gpadc_channel_exit(hal_gpadc_channel_t channal);
hal_gpadc_status_t hal_gpadc_register_callback(hal_gpadc_channel_t channal,
        gpadc_callback_t user_callback);

#ifdef __cplusplus
}
#endif

#endif
