
/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.


 * THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
 * PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
 * THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
 * OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*
 * ===========================================================================================
 *
 *  Filename:  hal_pwm.c
 *
 *  Description:   pwm driver core hal,be used by drv_pwm.c
 *
 *  Version:  Melis3.0
 *  Create:  2019-12-23
 *  Revision:  none
 *  Compiler:  GCC:version 9.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *  Author:  liuyus@allwinnertech.com
 *  Organization:  SWC-BPD
 *  Last Modified:  2019-12-31 17:55
 *
 * ===========================================================================================
 */

#include <log.h>
#include <stdio.h>
#include <stdint.h>
#include <hal_clk.h>
#include <sunxi_hal_gpio.h>
#include <sunxi_hal_common.h>
#include <sunxi_hal_pwm.h>
#include <script.h>

#define SET_REG_VAL(reg_val, shift, width, set_val)     ((reg_val & ~((-1UL) >> (32 - width) << shift)) | (set_val << shift))

/************** config *************************/
/*
*   pwm_set_clk_src(): pwm clock source selection
*
*   @channel_in: pwm channel number
*       pwm01 pwm23 pwm45 pwm67 pwm89
*
*   @clk_src: The clock you want to set
*       0:OSC24M  1:APB1
*/
void hal_pwm_clk_src_set(uint32_t channel_in, hal_pwm_clk_src clk_src)
{
    uint32_t reg_addr = PWM_BASE + PWM_PCCR_BASE ;
    uint32_t reg_val;

    uint32_t channel = channel_in / 2;
    reg_addr += 4 * channel;
    /*set clock source OSC24M or apb1*/
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, PWM_CLK_SRC_SHIFT, PWM_CLK_SRC_WIDTH, clk_src);
    hal_writel(reg_val, reg_addr);
}
/*
*   pwm_clk_div_m(): pwm clock divide
*
*   @div_m: 1 2 4 8 16 32 64 128 256
*/
void hal_pwm_clk_div_m(uint32_t channel_in, uint32_t div_m)
{
    uint32_t reg_addr = PWM_BASE + PWM_PCCR_BASE;
    uint32_t reg_val;

    uint32_t channel = channel_in / 2;
    reg_addr += 4 * channel;
    /*set clock div_m*/
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, PWM_DIV_M_SHIFT, PWM_DIV_M_WIDTH, div_m);
    hal_writel(reg_val, reg_addr);

}


void hal_pwm_prescal_set(uint32_t channel_in, uint32_t prescal)
{
    uint32_t reg_addr = PWM_BASE + PWM_PCR;
    uint32_t reg_val;

    uint32_t channel = channel_in;
    reg_addr += 0x20 * channel;
    /*set prescal*/
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, PWM_PRESCAL_SHIFT, PWM_PRESCAL_WIDTH, prescal);
    hal_writel(reg_val, reg_addr);
}

/* active cycles  */
void hal_pwm_set_active_cycles(uint32_t channel_in, uint32_t active_cycles)  //64
{
    uint32_t reg_addr = PWM_BASE + PWM_PPR ;
    uint32_t reg_val;

    uint32_t channel = channel_in;
    reg_addr += 0x20 * channel;
    /*set active*/
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, PWM_ACTIVE_CYCLES_SHIFT, PWM_ACTIVE_CYCLES_WIDTH, active_cycles);
    hal_writel(reg_val, reg_addr);
}

/* entire cycles */
void hal_pwm_set_period_cycles(uint32_t channel_in, uint32_t period_cycles)
{
    uint32_t reg_addr = PWM_BASE + PWM_PPR ;
    uint32_t reg_val;

    uint32_t channel = channel_in;
    reg_addr += 0x20 * channel;
    /*set clock BYPASS*/
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, PWM_PERIOD_SHIFT, PWM_PERIOD_WIDTH, period_cycles);
    hal_writel(reg_val, reg_addr);

}

/************   enable  **************/

void hal_pwm_enable_clk_gating(uint32_t channel_in)
{
    uint32_t reg_addr = PWM_BASE + PWM_PCGR;
    uint32_t reg_val;

    uint32_t channel = channel_in;
    /*enable clk_gating*/
    //reg_val = *(uint32_t *)((char *) reg_addr);
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, (PWM_CLK_GATING_SHIFT + channel), PWM_CLK_GATING_WIDTH, 1);
    hal_writel(reg_val, reg_addr);
}

void hal_pwm_enable_controller(uint32_t channel_in)
{
    uint32_t reg_addr = PWM_BASE + PWM_PER;
    uint32_t reg_val;

    uint32_t channel = channel_in;
    /*enable pwm*/
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, (PWM_EN_CONTROL_SHIFT + channel), PWM_EN_CONTORL_WIDTH, 1);
    hal_writel(reg_val, reg_addr);
}

/************   disable  **************/
void hal_pwm_disable_controller(uint32_t channel_in)
{
    uint32_t reg_val;
    uint32_t reg_addr = PWM_BASE + PWM_PER;

    uint32_t channel = channel_in;
    /*enable pwm*/
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, (PWM_EN_CONTROL_SHIFT + channel), PWM_EN_CONTORL_WIDTH, 0);
    hal_writel(reg_val, reg_addr);
}


/*************** polarity *****************/
void hal_pwm_porality(uint32_t channel_in, hal_pwm_polarity polarity)
{
    uint32_t reg_val;
    uint32_t reg_addr = PWM_BASE + PWM_PCR;

    uint32_t channel = channel_in;
    reg_addr += 0x20 * channel;
    /*set polarity*/
    reg_val = hal_readl(reg_addr);
    reg_val = SET_REG_VAL(reg_val, PWM_ACT_STA_SHIFT, PWM_ACT_STA_WIDTH, polarity);
    hal_writel(reg_val, reg_addr);
}


/****the function provide for pwm driverr******************************************/

pwm_status_t hal_pwm_init(void)
{
    PWM_INFO("pwm init start");

    if (hal_clock_enable(HAL_CLK_PERIPH_PWM))
    {
        return -1;
    }

    return 0;
}


pwm_status_t hal_pwm_control(int channel, struct pwm_config *config_pwm)
{
    void *phandle;
    script_gpio_set_t gpio_cfg = {0};
    char pwm_name[10];
    int ret = 0;

    PWM_INFO("pwm control start");
    //struct pwm_config * config_pwm = (struct pwm_config *)args;

    unsigned long long c = 0;
    unsigned long entire_cycles = 100000000, active_cycles = 10000000;

    PWM_INFO("period_ns = %d", config_pwm->period_ns);
    PWM_INFO("duty_ns = %d", config_pwm->duty_ns);
    PWM_INFO("polarity = %d", config_pwm->polarity);
    PWM_INFO("channel = %d", channel);

    /***pwm_channel enable***********/
    /* port set */

    phandle = script_get_handle();
    if(phandle == NULL) {
	    PWM_ERR("parse error, fex not initialized.\n");
	    return -1;
    }

    memset(&pwm_name, 0, sizeof(pwm_name));
    sprintf(pwm_name, "pwm%d", channel);
    ret = script_parser_fetch(phandle, pwm_name, "pwm_positive",
		    (int *)&gpio_cfg, sizeof(script_gpio_set_t) >> 2);
    if (ret != SCRIPT_PARSER_OK) {
	    PWM_ERR("get GPIO failed.\n");
	    return -1;
    }
    hal_gpio_pinmux_set_function((gpio_cfg.port - 1) * 32 + gpio_cfg.port_num,
		    gpio_cfg.mul_sel);
    /*enable clk gating*/
    hal_pwm_enable_clk_gating(channel);
    /*enable pwm controller*/
    hal_pwm_enable_controller(channel);

    /*******pwm set polarity*************************/
    hal_pwm_porality(channel, config_pwm->polarity);

    /********pwm config*****************************************/
    uint32_t pre_scal_id = 0, div_m = 0, prescale = 0;
    uint32_t pre_scal[][2] =
    {
        /*reg_val   clk_pre_div*/
        {0, 1},
        {1, 2},
        {2, 4},
        {3, 8},
        {4, 16},
        {5, 32},
        {6, 64},
        {7, 128},
        {8, 256},
    };

    if (config_pwm->period_ns > 0 && config_pwm->period_ns <= 10)
    {
        return -1;
    }
    else if (config_pwm->period_ns > 10 && config_pwm->period_ns <= 334)
    {
        /* if freq between 3M~100M, then select 100M as clock */
        c = 100000000;
        /*set src apb1*/
        hal_pwm_clk_src_set(channel, 1);
    }
    else
    {
        /* if freq < 3M, then select 24M clock */
        c = 24000000;
        /*set src osc24*/
        hal_pwm_clk_src_set(channel, 0);
    }

    c = c * (config_pwm->period_ns) / 1000000000;
    entire_cycles = (unsigned long)c;

    /*check the div_m and prescale*/
    for (pre_scal_id = 0; pre_scal_id < 9; pre_scal_id++)
    {
        if (entire_cycles <= 65536)
        {
            break;
        }
        for (prescale = 0; prescale < 256; prescale++)
        {
            entire_cycles = ((unsigned long)c / pre_scal[pre_scal_id][1]) / (prescale + 1);
            if (entire_cycles <= 65536)
            {
                div_m = pre_scal[pre_scal_id][0];
                break;
            }
        }
    }

    c = (unsigned long long)entire_cycles * (config_pwm->duty_ns);
    active_cycles = c / (config_pwm->period_ns);

    if (entire_cycles == 0)
    {
        entire_cycles++;
    }

    /*config div_m*/
    hal_pwm_clk_div_m(channel, div_m);

    /*config prescale*/
    hal_pwm_prescal_set(channel, prescale);

    /*config active_cycles*/
    hal_pwm_set_active_cycles(channel, active_cycles);

    /*config entire_cycles*/
    hal_pwm_set_period_cycles(channel, entire_cycles);

    return 0;
}

pwm_status_t hal_pwm_uninit(void)
{
    PWM_INFO("pwm uninit start");

    if (hal_clock_disable(HAL_CLK_PERIPH_PWM))
    {
        return -1;
    }

    return 0;
}

const sunxi_hal_driver_pwm_t sunxi_hal_pwm_driver =
{
    .initialize     = hal_pwm_init,
    .control        = hal_pwm_control,
    .uninitialize   = hal_pwm_uninit,
};












