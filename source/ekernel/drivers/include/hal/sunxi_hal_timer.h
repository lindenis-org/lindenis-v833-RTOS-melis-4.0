/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY¡¯S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS¡¯SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY¡¯S TECHNOLOGY.
*
*
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

#ifndef SUXNI_HAL_TIMER_H
#define SUXNI_HAL_TIMER_H

#include "sunxi_hal_common.h"
#include <interrupt.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_DRIVERS_TMR_DEBUG

#ifdef CONFIG_DRIVERS_TMR_DEBUG
#define TMR_INFO(fmt, arg...) printf("GPIO : %s()%d "fmt, __func__, __LINE__, ##arg)
#define TMR_ERR(fmt, arg...) printf("GPIO : %s()%d "fmt, __func__, __LINE__, ##arg)
#else
#define TMR_INFO(fmt, arg...) do {}while(0)
#define TMR_ERR(fmt, arg...) do {}while(0)
#endif

#define TMR_BASE_REG 0x03009000

#define TMR_IRQ_EN_REG (TMR_BASE_REG + 0x0000)
#define TMR_IRQ_STA_REG (TMR_BASE_REG + 0x0004)
#define TMR0_CTRL_REG (TMR_BASE_REG + 0x0010)
#define TMR0_INTV_VALUE_REG (TMR_BASE_REG + 0x0014)
#define TMR0_CUR_VALUE_REG (TMR_BASE_REG + 0x0018)
#define TMR1_CTRL_REG (TMR_BASE_REG + 0x0020)
#define TMR1_INTV_VALUE_REG (TMR_BASE_REG + 0x0024)
#define TMR1_CUR_VALUE_REG (TMR_BASE_REG + 0x0028)
#define TMR_VER_REG (TMR_BASE_REG + 0x0090)

#define SUNXI_GIC_START 32
#define SUNXI_IRQ_TMR0 (SUNXI_GIC_START + 60)
#define SUNXI_IRQ_TMR1 (SUNXI_GIC_START + 61)

#define INTV_DEFAULT_VAL 0x2EE0
#define INTV_DEFAULT_CTRL 0x0094

#define TIMER_NUM 2
#define TIME_MODE_NUM 2
#define TMR_PRES_NUM 8

struct timer_irq_desc
{
    u32 irq;
    unsigned long flags;
    irq_handler_t irq_handle;
    void *data;
};

struct hal_sunxi_timer
{
    long clk;
    u32 timer_id;
    u32 ctrl_reg;
    u32 intv_reg;
    u32 cur_value_reg;
    struct timer_irq_desc irq_desc;
};

int hal_check_valid_timer_id(u32 timer_id);
int hal_check_valid_timer_mode(u32 timer_mode);
int hal_check_valid_timer_pres(u32 pres_mode);
int hal_timer_set_pres(u32 timer_id, u32 pres_mode);
int hal_timer_set_mode(u32 timer_id, u32 timer_mode);
int hal_timer_get_counter(u32 timer_id, u32 *value);
int hal_timer_set_counter(u32 timer_id, u32 value);
int hal_timer_set_interval(u32 timer_id, u32 value);
int hal_timer_put_start_reg(u32 timer_id);
int hal_timer_start(u32 timer_id);
int hal_timer_enable(u32 timer_id);
u32 hal_timer_id_to_irq(u32 timer_id);
int hal_timer_irq_request(u32 irq, irq_handler_t hdle, unsigned long flags, void *data);
int hal_timer_irq_free(u32 irq);
int hal_timer_irq_enable(u32 irq);
int hal_timer_irq_disable(u32 irq);
int hal_timer_init(void);

#ifdef __cplusplus
}
#endif
#endif
