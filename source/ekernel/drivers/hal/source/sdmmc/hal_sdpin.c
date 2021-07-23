/**
  * @file  hal_sdhost.c
  * @author  ALLWINNERTECH IOT WLAN Team
  */

/*
 * Copyright (C) 2017 ALLWINNERTECH TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of ALLWINNERTECH TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, )|hhst->sdio_irq_maskPROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sunxi_hal_gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <log.h>
#include "_sdhost.h"

typedef struct
{
    uint32_t *pin;
    uint8_t pin_num : 3;
    uint8_t pin_mux : 3;
    uint8_t pin_drv : 2;
} sdmmc_pin_t;

static sdmmc_pin_t sunxi_sdmmc_pin[4];

#define SDMMC_ERR(fmt, arg...) printf("%s()%d " fmt, __func__, __LINE__, ##arg)

#define SDMMC_MUXSEL 2
#define SDMMC_DRVSEL 3
#define SDMMC_PULL GPIO_PULL_DOWN_DISABLE

/*sdc0 pin*/
#define SDC0_NUM 6
#define SDC0_CLK    GPIO_PF2
#define SDC0_CMD    GPIO_PF3
#define SDC0_D0    GPIO_PF1
#define SDC0_D1    GPIO_PF0
#define SDC0_D2    GPIO_PF5
#define SDC0_D3    GPIO_PF4
#define SDC0_DET    GPIO_PF6

/*sdc1 pin*/
#define SDC1_NUM 6
#define SDC1_CLK    GPIO_PG0
#define SDC1_CMD    GPIO_PG1
#define SDC1_D0    GPIO_PG2
#define SDC1_D1    GPIO_PG3
#define SDC1_D2    GPIO_PG4
#define SDC1_D3    GPIO_PG5


uint32_t sdmmc_pinctrl_init(struct mmc_host *host)
{
    uint8_t i;
    uint32_t flags = 0;
    int ret;
    uint32_t host_id = host->sdc_id;

    switch (host_id)
    {
        case 0:
            sunxi_sdmmc_pin[host_id].pin_num = SDC0_NUM;
            sunxi_sdmmc_pin[host_id].pin_mux = SDMMC_MUXSEL;
            sunxi_sdmmc_pin[host_id].pin_drv = SDMMC_DRVSEL;
            sunxi_sdmmc_pin[host_id].pin = malloc(sizeof(uint32_t) * SDC0_NUM);
            sunxi_sdmmc_pin[host_id].pin[0] = SDC0_CLK;
            sunxi_sdmmc_pin[host_id].pin[1] = SDC0_CMD;
            sunxi_sdmmc_pin[host_id].pin[2] = SDC0_D0;
            sunxi_sdmmc_pin[host_id].pin[3] = SDC0_D1;
            sunxi_sdmmc_pin[host_id].pin[4] = SDC0_D2;
            sunxi_sdmmc_pin[host_id].pin[5] = SDC0_D3;
            break;
        case 1:
	    if (host->param.pwr_mode == POWER_MODE_330) {
		hal_gpio_sel_vol_mode(SDC1_D0, POWER_MODE_330);
	    } else {
		hal_gpio_sel_vol_mode(SDC1_D0, POWER_MODE_180);
	    }
            sunxi_sdmmc_pin[host_id].pin_num = SDC1_NUM;
            sunxi_sdmmc_pin[host_id].pin_mux = SDMMC_MUXSEL;
            sunxi_sdmmc_pin[host_id].pin_drv = SDMMC_DRVSEL;
            sunxi_sdmmc_pin[host_id].pin = malloc(sizeof(uint32_t) * SDC1_NUM);
            sunxi_sdmmc_pin[host_id].pin[0] = SDC1_CLK;
            sunxi_sdmmc_pin[host_id].pin[1] = SDC1_CMD;
            sunxi_sdmmc_pin[host_id].pin[2] = SDC1_D0;
            sunxi_sdmmc_pin[host_id].pin[3] = SDC1_D1;
            sunxi_sdmmc_pin[host_id].pin[4] = SDC1_D2;
            sunxi_sdmmc_pin[host_id].pin[5] = SDC1_D3;
            break;
        default:
            SDMMC_ERR("sdmmc%ld is invalid\n", host_id);
            return -1;

    }

    for (i = 0; i < sunxi_sdmmc_pin[host_id].pin_num; i++)
    {
        ret = hal_gpio_pinmux_set_function(sunxi_sdmmc_pin[host_id].pin[i], sunxi_sdmmc_pin[host_id].pin_mux);
        if (ret)
        {
            SDMMC_ERR(
                "[sdmmc%ld] PIN%lu set function failed! return %d\n",
                host_id, sunxi_sdmmc_pin[host_id].pin[i], ret);
            return -1;
        }
        ret = hal_gpio_set_driving_level(sunxi_sdmmc_pin[host_id].pin[i], sunxi_sdmmc_pin[host_id].pin_drv);
        if (ret)
        {
            SDMMC_ERR(
                "[sdmmc%ld] PIN%lu set driving level failed! return %d\n",
                host_id, sunxi_sdmmc_pin[host_id].pin[i], ret);
            return -1;
        }
        ret = hal_gpio_set_pull(sunxi_sdmmc_pin[host_id].pin[i], GPIO_PULL_UP);
        // ret = drv_gpio_set_pull_state(sunxi_sdmmc_pin[host_id].pin[i], DRV_GPIO_PULL_DOWN_DISABLE);

    }

    return 0;
}

int mmc_gpiod_request_cd_irq(struct mmc_host *host)
{
    uint32_t irq;
    int ret = 0;
    gpio_pull_status_t pull_state;
    gpio_direction_t gpio_direction;
    gpio_data_t gpio_data;

    host->cd_gpio_pin = SDC0_DET;
    /*set gpio detect-clk 24M*/
    hal_gpio_set_debounce(host->cd_gpio_pin, 1);
    ret = hal_gpio_to_irq(host->cd_gpio_pin, &irq);
    if (ret < 0)
    {
        printf("gpio to irq error, error num: %d\n", ret);
        return ret;
    }

    /*set pin mux*/
    ret = hal_gpio_pinmux_set_function(host->cd_gpio_pin, 0);
    ret = hal_gpio_set_driving_level(host->cd_gpio_pin, 3);
    ret = hal_gpio_set_pull(host->cd_gpio_pin, 1);
    if (ret < 0)
    {
        printf("set pin mux error!\n");
        return -1;
    }

    host->cd_irq = irq;
    ret = hal_gpio_irq_request(irq, host->cd_gpio_isr, IRQ_TYPE_EDGE_BOTH, host);
    if (ret < 0)
    {
        printf("request irq error, irq num:%lu error num: %d\n", (unsigned long)irq, ret);
        return ret;
    }

#if 0
    ret = drv_gpio_irq_enable(irq);
    if (ret < 0)
    {
        printf("request irq error, error num: %d\n", ret);
        return ret;
    }
    ret = drv_gpio_irq_disable(irq);
    if (ret < 0)
    {
        printf("disable irq error, irq num:%lu,error num: %d\n", irq, ret);
        return ret;
    }

    ret = drv_gpio_irq_free(irq);
    if (ret < 0)
    {
        printf("free irq error, error num: %d\n", ret);
        return ret;
    }
#endif
    return ret;
}

