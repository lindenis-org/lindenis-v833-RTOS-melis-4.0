/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
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

#include <sunxi_hal_timer.h>
#include <interrupt.h>
#include <stdlib.h>
#include <stdio.h>


struct hal_sunxi_timer *hal_timer = NULL;

static const int hal_timer_irq_num[] =
{
    SUNXI_IRQ_TMR0,
    SUNXI_IRQ_TMR1,
};

static const int hal_timer_ctrl_reg[] =
{
    TMR0_CTRL_REG,
    TMR1_CTRL_REG,
};

static const int hal_timer_intv_reg[] =
{
    TMR0_INTV_VALUE_REG,
    TMR1_INTV_VALUE_REG,
};

static const int hal_timer_cur_value_reg[] =
{
    TMR0_CUR_VALUE_REG,
    TMR1_CUR_VALUE_REG,
};

int hal_check_valid_timer_id(u32 timer_id)
{
    if (timer_id >= TIMER_NUM)
    {
        return -1;
    }
    return 0;
}

int hal_check_valid_timer_mode(u32 timer_mode)
{
    if (timer_mode >= TIME_MODE_NUM)
    {
        return -1;
    }
    return 0;
}

int hal_check_valid_timer_pres(u32 pres_mode)
{
    if (pres_mode >= TMR_PRES_NUM)
    {
        return -1;
    }
    return 0;
}

u32 hal_timer_id_to_irq(u32 timer_id)
{
    return hal_timer[timer_id].irq_desc.irq;
}

static u32 irq_to_timer_id(u32 irq)
{
    int i;
    for (i = 0; i < TIMER_NUM; i++)
    {
        if (hal_timer[i].irq_desc.irq == irq)
        {
            return i;
        }
    }
    return i;
}

int hal_timer_set_pres(u32 timer_id, u32 pres_mode)
{
    u32 val;
    if (timer_id >= TIMER_NUM || pres_mode >= TMR_PRES_NUM)
    {
        return -1;
    }

    val = readl(hal_timer[timer_id].ctrl_reg);
    val &= ~(0x7 << 4);
    val |= (pres_mode << 4);
    writel(val, hal_timer[timer_id].ctrl_reg);

    return 0;
}

int hal_timer_set_mode(u32 timer_id, u32 timer_mode)
{
    u32 val;
    if (timer_id >= TIMER_NUM || timer_mode >= TIME_MODE_NUM)
    {
        return -1;
    }

    val = readl(hal_timer[timer_id].ctrl_reg);
    val &= ~(1 << 7);
    val |= (timer_mode << 7);
    writel(val, hal_timer[timer_id].ctrl_reg);

    return 0;
}


int hal_timer_get_counter(u32 timer_id, u32 *value)
{
    if (timer_id >= TIMER_NUM)
    {
        return -1;
    }

    *value = readl(hal_timer[timer_id].cur_value_reg);
    return 0;
}

int hal_timer_set_counter(u32 timer_id, u32 value)
{
    if (timer_id >= TIMER_NUM)
    {
        return -1;
    }

    writel(value, hal_timer[timer_id].cur_value_reg);
    return 0;
}

int hal_timer_set_interval(u32 timer_id, u32 value)
{
    if (timer_id >= TIMER_NUM)
    {
        return -1;
    }

    writel(value, hal_timer[timer_id].intv_reg);
    return 0;
}

int hal_timer_put_start_reg(u32 timer_id)
{
    u32 val;

    if (timer_id >= TIMER_NUM)
    {
        return -1;
    }

    val = readl(hal_timer[timer_id].ctrl_reg);
    val |= (0x01 << 1);
    writel(val, hal_timer[timer_id].ctrl_reg);
    while ((readl(hal_timer[timer_id].ctrl_reg) >> 1) & 1) ;
    return 0;
}

int hal_timer_start(u32 timer_id)
{
    u32 val;

    if (timer_id >= TIMER_NUM)
    {
        return -1;
    }

    val = readl(hal_timer[timer_id].ctrl_reg);
    val |= 0x01;
    writel(val, hal_timer[timer_id].ctrl_reg);

    return 0;
}

int hal_timer_enable(u32 timer_id)
{
    u32 val;

    if (timer_id >= TIMER_NUM)
    {
        return -1;
    }

    val = readl(TMR_BASE_REG);
    val |= (1 << timer_id);
    writel(val, TMR_BASE_REG);

    return 0;
}

static irqreturn_t hal_timer_bad_irq_handle(int dummy, void *data)
{
    TMR_INFO("Timer irq interrupt!!\n");
    return 0;
}

static irqreturn_t hal_timer_irq_handle(int dummy, void *data)
{
    u32 irq = *((u32 *)data);
    u32 timer_id = irq_to_timer_id(irq);
    u32 val = 0;

    val = readl(TMR_IRQ_STA_REG) >> timer_id;

    if (val & 0x1)
    {
        hal_timer[timer_id].irq_desc.irq_handle(hal_timer[timer_id].irq_desc.irq,
                                                hal_timer[timer_id].irq_desc.data);
        /*clear pending*/
        writel(0x1 << timer_id, TMR_IRQ_STA_REG);
    }

    return 0;
}

int hal_timer_irq_request(u32 irq, irq_handler_t hdle, unsigned long flags, void *data)
{
    u32 timer_id = irq_to_timer_id(irq);
    if (timer_id == TIMER_NUM || hdle == NULL)
    {
        return -1;
    }
    hal_timer[timer_id].irq_desc.irq_handle = hdle;
    hal_timer[timer_id].irq_desc.flags = flags;
    hal_timer[timer_id].irq_desc.data = data;
    return 0;
}

int hal_timer_irq_free(u32 irq)
{
    u32 timer_id = irq_to_timer_id(irq);
    if (timer_id == TIMER_NUM)
    {
        return -1;
    }
    hal_timer[timer_id].irq_desc.irq_handle = hal_timer_irq_handle;
    hal_timer[timer_id].irq_desc.flags = 0;
    hal_timer[timer_id].irq_desc.data = NULL;
    return 0;
}

int hal_timer_irq_enable(u32 irq)
{
    u32 val = 0;
    u32 timer_id = irq_to_timer_id(irq);

    if (timer_id == TIMER_NUM)
    {
        return -1;
    }

    /*clear pending*/
    writel(1 << timer_id, TMR_IRQ_STA_REG);

    /*put the intv reg data to the cur data reg*/
    hal_timer_put_start_reg(timer_id);

    /*start timer*/
    hal_timer_start(timer_id);

    /*enable interrupt*/
    hal_timer_enable(timer_id);


    return 0;
}

int hal_timer_irq_disable(u32 irq)
{
    u32 val;
    u32 timer_id = irq_to_timer_id(irq);
    if (timer_id == TIMER_NUM)
    {
        return -1;
    }

    val = readl(TMR_BASE_REG);
    val &= ~(1 << timer_id);
    writel(val, TMR_BASE_REG);
    return 0;
}

int hal_timer_init(void)
{
    int i, ret;

    hal_timer = (struct hal_sunxi_timer *)malloc(TIMER_NUM * sizeof(struct hal_sunxi_timer));
    if (hal_timer == NULL)
    {
        TMR_ERR("time malloc error\n");
    }

    /*init the timer struct*/
    for (i = 0; i < TIMER_NUM; i++)
    {
        hal_timer[i].timer_id = i;
        hal_timer[i].clk = 200000000;
        hal_timer[i].ctrl_reg = hal_timer_ctrl_reg[i];
        hal_timer[i].intv_reg = hal_timer_intv_reg[i];
        hal_timer[i].cur_value_reg = hal_timer_cur_value_reg[i];
        hal_timer[i].irq_desc.irq = hal_timer_irq_num[i];
        hal_timer[i].irq_desc.irq_handle = hal_timer_bad_irq_handle;
    }

    /*init the timer interrupt*/
    for (i = 0; i < TIMER_NUM; i++)
    {
        ret = request_irq(hal_timer[i].irq_desc.irq, hal_timer_irq_handle, 0, NULL, (void *)&hal_timer_irq_num[i]);
        if (ret < 0)
        {
            TMR_ERR("time%d request irq error\n", i);
            free(hal_timer);
            return ret;
        }
        enable_irq(hal_timer[i].irq_desc.irq);
    }

    /*init the timer default config*/
    for (i = 0; i < TIMER_NUM; i++)
    {
        writel(INTV_DEFAULT_VAL, hal_timer[i].intv_reg);
        writel(INTV_DEFAULT_CTRL, hal_timer[i].ctrl_reg);
    }
    return 0;
}
