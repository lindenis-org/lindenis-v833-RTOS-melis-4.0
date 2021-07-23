/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY��S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS��SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY��S TECHNOLOGY.
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
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <hal_cache.h>
#include <hal_mem.h>
#include <hal_atomic.h>
#include <hal_clk.h>
#include <interrupt.h>
#include <hal_osal.h>
#include <interrupt.h>
#include "sunxi_dma_core.h"
#include <arch.h>

#define DMA_ERR(fmt, arg...) printf("%s()%d " fmt, __func__, __LINE__, ##arg)

static struct sunxi_dma_chan    dma_chan_source[NR_MAX_CHAN];

/*
 * Fix sconfig's bus width according to at_dmac.
 * 1 byte -> 0, 2 bytes -> 1, 4 bytes -> 2, 8bytes -> 3
 */
static inline uint32_t convert_buswidth(enum dma_slave_buswidth addr_width)
{
    if (addr_width > DMA_SLAVE_BUSWIDTH_8_BYTES)
    {
        return 0;
    }

    switch (addr_width)
    {
        case DMA_SLAVE_BUSWIDTH_2_BYTES:
            return 1;
        case DMA_SLAVE_BUSWIDTH_4_BYTES:
            return 2;
        case DMA_SLAVE_BUSWIDTH_8_BYTES:
            return 3;
        default:
            /* For 1 byte width or fallback */
            return 0;
    }
}

static inline void convert_burst(uint32_t *maxburst)
{
    switch (*maxburst)
    {
        case 1:
            *maxburst = 0;
            break;
        case 4:
            *maxburst = 1;
            break;
        case 8:
            *maxburst = 2;
            break;
        case 16:
            *maxburst = 3;
            break;
        default:
            printf("unknown maxburst\n");
            *maxburst = 0;
            break;
    }
}

static inline void sunxi_cfg_lli(struct sunxi_dma_lli *lli, uint32_t src_addr, uint32_t dst_addr, uint32_t len, struct dma_slave_config *config)
{
    uint32_t src_width = 0, dst_width = 0;

    if (NULL == lli && NULL == config)
    {
        return;
    }

    src_width = convert_buswidth(config->src_addr_width);
    dst_width = convert_buswidth(config->dst_addr_width);
    lli->cfg = SRC_BURST(config->src_maxburst) | \
               SRC_WIDTH(src_width) | \
               DST_BURST(config->dst_maxburst) | \
               DST_WIDTH(dst_width);

    lli->src = src_addr;
    lli->dst = dst_addr;
    lli->len = len;
    lli->para = NORMAL_WAIT;
}
static void sunxi_dump_lli(struct sunxi_dma_chan *chan, struct sunxi_dma_lli *lli)
{
#ifdef DMA_DEBUG
    printf("channum:%x\n"
           "\t\tdesc:desc - 0x%08lx desc p - 0x%08x desc v - 0x%08lx\n"
           "\t\tlli: v- 0x%08lx cfg - 0x%08lx s - 0x%08x d - 0x%08x\n"
           "\t\tlen - 0x%08x para - 0x%08x p_lln - 0x%08x\n",
           chan->chan_count, (uint32_t)chan->desc, (uint16_t)chan->desc->p_lln,
           (uint32_t)chan->desc->vlln, (uint32_t)lli->vlln, (uint32_t)lli->cfg, (uint16_t)lli->src,
           (uint16_t)lli->dst, (uint16_t)lli->len, (uint16_t)lli->para, (uint16_t)lli->p_lln);
#endif
}

static void sunxi_dump_com_regs(void)
{
#ifdef DMA_DEBUG
    printf("Common register:\n"
           "\tmask0: 0x%08lx\n"
           "\tmask1: 0x%08lx\n"
           "\tpend0: 0x%08lx\n"
           "\tpend1: 0x%08lx\n"
#ifdef DMA_SECURE
           "\tsecur: 0x%08lx\n"
#endif
#ifdef DMA_GATE
           "\t_gate: 0x%08lx\n"
#endif
           "\tstats: 0x%08lx\n",
           hal_readl(DMA_IRQ_EN(0)),
           hal_readl(DMA_IRQ_EN(1)),
           hal_readl(DMA_IRQ_STAT(0)),
           hal_readl(DMA_IRQ_STAT(1)),
#ifdef DMA_SECURE
           hal_readl(DMA_SECURE),
#endif
#ifdef DMA_GATE
           hal_readl(DMA_GATE),
#endif
           hal_readl(DMA_STAT));
#endif
}
static inline void sunxi_dump_chan_regs(struct sunxi_dma_chan *ch)
{
#ifdef DMA_DEBUG
    u32 chan_num = ch->chan_count;
    printf("Chan %ld reg:\n"
           "\t___en: \t0x%08lx\n"
           "\tpause: \t0x%08lx\n"
           "\tstart: \t0x%08lx\n"
           "\t__cfg: \t0x%08lx\n"
           "\t__src: \t0x%08lx\n"
           "\t__dst: \t0x%08lx\n"
           "\tcount: \t0x%08lx\n"
           "\t_para: \t0x%08lx\n\n",
           chan_num,
           hal_readl(DMA_ENABLE(chan_num)),
           hal_readl(DMA_PAUSE(chan_num)),
           hal_readl(DMA_LLI_ADDR(chan_num)),
           hal_readl(DMA_CFG(chan_num)),
           hal_readl(DMA_CUR_SRC(chan_num)),
           hal_readl(DMA_CUR_DST(chan_num)),
           hal_readl(DMA_CNT(chan_num)),
           hal_readl(DMA_PARA(chan_num)));
#endif
}
static void *sunxi_lli_list(struct sunxi_dma_lli *prev, struct sunxi_dma_lli *next, struct sunxi_dma_chan *chan)
{
    if ((!prev && !chan) || !next)
    {
        return NULL;
    }
    if (!prev)
    {
        chan->desc = next;
        chan->desc->p_lln = __va_to_pa((uint32_t)next);
        chan->desc->vlln = next;
    }
    else
    {
        prev->p_lln = __va_to_pa((uint32_t)next);
        prev->vlln = next;
    }

    next->p_lln = LINK_END;
    next->vlln = NULL;

    return next;
}

static irqreturn_t sunxi_dma_irq_handle(int irq,  void *ptr)
{

    uint32_t status_l = 0, status_h = 0;
    int i = 0;

    status_l = hal_readl(DMA_IRQ_STAT(0));
#if NR_MAX_CHAN > HIGH_CHAN
    status_h = hal_readl(DMA_IRQ_STAT(1));
#endif
    hal_writel(status_l, DMA_IRQ_STAT(0));
#if NR_MAX_CHAN > HIGH_CHAN
    hal_writel(status_h, DMA_IRQ_STAT(1));
#endif

    for (i = 0; i < NR_MAX_CHAN; i++)
    {
        uint32_t __cpsr;
        struct sunxi_dma_chan *chan = &dma_chan_source[i];

        __cpsr = hal_spin_lock_irqsave();
        uint32_t chan_num = chan->chan_count;
        uint32_t status = 0;

        if (chan->used == 0)
        {
            hal_spin_unlock_irqrestore(__cpsr);
            continue;
        }

        status = (chan_num >= HIGH_CHAN) \
                 ? (status_h >> ((chan_num - HIGH_CHAN) << 2)) \
                 : (status_l >> (chan_num << 2));

        if (!(chan->irq_type & status))
        {
            goto unlock;
        }

        if (chan->cyclic)
        {
            dma_callback cb = NULL;
            void *cb_data = NULL;

            chan->periods_pos ++;
            if (chan->periods_pos * chan->desc->len >= chan->buf_len)
            {
                chan->periods_pos = 0;
            }
            cb = chan->callback;
            cb_data = chan->callback_param;

            hal_spin_unlock_irqrestore(__cpsr);
            if (cb)
            {
                cb(cb_data);
            }
            __cpsr = hal_spin_lock_irqsave();
        }
        else
        {
            dma_callback cb = NULL;
            void *cb_data = NULL;

            cb = chan->callback;
            cb_data = chan->callback_param;

            hal_spin_unlock_irqrestore(__cpsr);
            if (cb)
            {
                cb(cb_data);
            }
            __cpsr = hal_spin_lock_irqsave();
        }
unlock:
        hal_spin_unlock_irqrestore(__cpsr);
    }
    return 0;
}

void sunxi_dma_init(void)
{
    uint32_t i = 0, high = 0;
    hal_clk_status_t ret = 0;

    memset((void *)dma_chan_source, 0, NR_MAX_CHAN * sizeof(struct sunxi_dma_chan));

    for (i = 0; i < NR_MAX_CHAN; i++)
    {
        /* krhino_spin_lock_init(&dma_chan_source[i].lock); */

        high = (i >= HIGH_CHAN) ? 1 : 0;

        /*disable all dma irq*/
        hal_writel(0, DMA_IRQ_EN(high));

        /*clear all dma irq pending*/
        hal_writel(0xffffffff, DMA_IRQ_STAT(high));
    }
    /*disable auto gating*/
    hal_writel(DMA_MCLK_GATE | DMA_COMMON_GATE | DMA_CHAN_GATE, DMA_GATE);

    hal_clock_enable(HAL_CLK_PERIPH_DMA);
    if (ret != HAL_CLK_STATUS_OK)
    {
        DMA_ERR("DMA clock enable failed \n");
    }
    /*request dma irq*/
    if (request_irq(DMA_IRQ_NUM, sunxi_dma_irq_handle, 0, "dma-ctl", NULL) < 0)
    {
        DMA_ERR("[dma] request irq error\n");
        /* SPI_ERR("[dma%d] request irq error\n", sspi->port); */
    }
    enable_irq(DMA_IRQ_NUM);
}

unsigned long sunxi_dma_chan_request(void)
{
    int i = 0;
    struct sunxi_dma_chan *chan;
    uint32_t __cpsr;
    for (i = 0; i < NR_MAX_CHAN; i++)
    {
        chan = &dma_chan_source[i];
        __cpsr = hal_spin_lock_irqsave();
        if (chan->used == 0)
        {
            chan->used = 1;
            chan->chan_count = i;
            hal_spin_unlock_irqrestore(__cpsr);
            return (unsigned long)&dma_chan_source[i];
        }
        hal_spin_unlock_irqrestore(__cpsr);
    }
    if (i == NR_MAX_CHAN)
	DMA_ERR("[dma] request failed, have not enough channel.\n");

    return 0;
}

void sunxi_dma_free_ill(struct sunxi_dma_chan *chan)
{
    struct sunxi_dma_lli *li_adr = NULL, *next = NULL;

    if (NULL == chan)
    {
        return;
    }

    li_adr = chan->desc;

    while (li_adr)
    {
        next = li_adr->vlln;
        hal_free(li_adr);
        li_adr = next;
    }

    chan->callback = NULL;
    chan->callback_param = NULL;
}

int sunxi_dma_chan_free(struct sunxi_dma_chan *chan)
{
    uint32_t high = 0;
    uint32_t irq_val = 0, pending_val = 0;
    uint32_t __cpsr;

    if (NULL == chan)
    {
        return -1;
    }

    if (!chan->used)
    {
        return -1;
    }

    MELIS_CPU_CRITICAL_ENTER();
    /* __cpsr = hal_spin_lock_irqsave(); */

    high = (chan->chan_count >= HIGH_CHAN) ? 1 : 0;

    irq_val = hal_readl(DMA_IRQ_EN(high));
    irq_val &= ~(SHIFT_IRQ_MASK(chan->irq_type, chan->chan_count));
    hal_writel(irq_val, DMA_IRQ_EN(high));

    sunxi_dma_free_ill(chan);

    chan->used = 0;
    MELIS_CPU_CRITICAL_LEAVE();
    /* hal_spin_unlock_irqrestore(__cpsr); */
    return 0;
}

int dma_slave_config(struct sunxi_dma_chan *chan, struct dma_slave_config *config)
{
    if (NULL == config || NULL == chan)
    {
        DMA_ERR("dma config is NULL\n");
        return -1;
    }

    uint32_t __cpsr;
    __cpsr = hal_spin_lock_irqsave();
    convert_burst(&config->src_maxburst);
    convert_burst(&config->dst_maxburst);
    memcpy((void *) & (chan->cfg), (void *)config, sizeof(struct dma_slave_config));
    hal_spin_unlock_irqrestore(__cpsr);
    return 0;
}

int sunxi_prep_dma_memcpy(struct sunxi_dma_chan *chan, uint32_t dest, uint32_t src, uint32_t len)
{
    struct sunxi_dma_lli *l_item = NULL;
    struct dma_slave_config *config = NULL;

    uint32_t __cpsr;
    if (NULL == chan)
    {
        return -1;
    }

    l_item = (struct sunxi_dma_lli *)hal_malloc(sizeof(struct sunxi_dma_lli));
    if (!l_item)
    {
        return -1;
    }
    memset(l_item, 0, sizeof(struct sunxi_dma_lli));

    __cpsr = hal_spin_lock_irqsave();
    config = &chan->cfg;

    dest = __va_to_pa(dest);
    src = __va_to_pa(src);
    sunxi_cfg_lli(l_item, src, dest, len, config);

    l_item->cfg |= SRC_DRQ(DRQSRC_SDRAM) \
                   | DST_DRQ(DRQDST_SDRAM) \
                   | DST_LINEAR_MODE \
                   | SRC_LINEAR_MODE;
    sunxi_lli_list(NULL, l_item, chan);

    sunxi_dump_lli(chan, l_item);

    hal_spin_unlock_irqrestore(__cpsr);

    return 0;
}

int sunxi_prep_dma_device(struct sunxi_dma_chan *chan, uint32_t dest, uint32_t src, uint32_t len, enum dma_transfer_direction dir)
{
    struct sunxi_dma_lli *l_item = NULL;
    struct dma_slave_config *config = NULL;
    uint32_t __cpsr;
    if (NULL == chan)
    {
        return -1;
    }

    l_item = (struct sunxi_dma_lli *)hal_malloc(sizeof(struct sunxi_dma_lli));
    if (!l_item)
    {
        return -1;
    }
    memset(l_item, 0, sizeof(struct sunxi_dma_lli));

    __cpsr = hal_spin_lock_irqsave();

    config = &chan->cfg;

    if (dir == DMA_MEM_TO_DEV)
    {
        src = __va_to_pa(src);
        sunxi_cfg_lli(l_item, src, dest, len, config);
        l_item->cfg |= GET_DST_DRQ(config->slave_id) \
                       | SRC_LINEAR_MODE \
                       | DST_IO_MODE \
                       | SRC_DRQ(DRQSRC_SDRAM);
    }
    else if (dir == DMA_DEV_TO_MEM)
    {
        dest = __va_to_pa(dest);
        sunxi_cfg_lli(l_item, src, dest, len, config);
        l_item ->cfg |= GET_SRC_DRQ(config->slave_id)  \
                        | DST_LINEAR_MODE \
                        | SRC_IO_MODE \
                        | DST_DRQ(DRQSRC_SDRAM);
    }
    else if (dir == DMA_DEV_TO_DEV)
    {
        sunxi_cfg_lli(l_item, src, dest, len, config);
        l_item->cfg |= GET_SRC_DRQ(config->slave_id) \
                       | DST_IO_MODE \
                       | SRC_IO_MODE \
                       | GET_DST_DRQ(config->slave_id);
    }

    sunxi_lli_list(NULL, l_item, chan);

    sunxi_dump_lli(chan, l_item);

    hal_spin_unlock_irqrestore(__cpsr);

    return 0;
}

int sunxi_prep_dma_cyclic(struct sunxi_dma_chan *chan, uint32_t buf_addr, uint32_t buf_len, uint32_t period_len, enum dma_transfer_direction dir)
{
    struct sunxi_dma_lli *l_item = NULL, *prev = NULL;
    uint32_t periods = buf_len / period_len;
    struct dma_slave_config *config = NULL;
    uint32_t i = 0;
    uint32_t __cpsr;

    if (NULL == chan && chan->cyclic)
    {
        return -1;
    }

    MELIS_CPU_CRITICAL_ENTER();
    /* __cpsr = hal_spin_lock_irqsave(); */

    config = &chan->cfg;
    for (i = 0; i < periods; i++)
    {
        l_item = (struct sunxi_dma_lli *)hal_malloc(sizeof(struct sunxi_dma_lli));
        if (!l_item)
        {
            hal_spin_unlock_irqrestore(__cpsr);
            return -1;
        }
        memset(l_item, 0, sizeof(struct sunxi_dma_lli));
        if (dir == DMA_MEM_TO_DEV)
        {
            sunxi_cfg_lli(l_item, __va_to_pa(buf_addr + period_len * i),
                          config->dst_addr, period_len, config);
            l_item->cfg |= GET_DST_DRQ(config->slave_id) \
                           | SRC_LINEAR_MODE \
                           | DST_IO_MODE \
                           | SRC_DRQ(DRQSRC_SDRAM);
        }
        else if (dir == DMA_DEV_TO_MEM)
        {
            sunxi_cfg_lli(l_item, config->src_addr, \
                          __va_to_pa(buf_addr + period_len * i), \
                          period_len, config);
            l_item ->cfg |= GET_SRC_DRQ(config->slave_id)  \
                            | DST_LINEAR_MODE \
                            | SRC_IO_MODE \
                            | DST_DRQ(DRQSRC_SDRAM);
        }
        else if (dir == DMA_DEV_TO_DEV)
        {
            sunxi_cfg_lli(l_item, config->src_addr, \
                          config->dst_addr, period_len, config);
            l_item->cfg |= GET_SRC_DRQ(config->slave_id) \
                           | DST_IO_MODE \
                           | SRC_IO_MODE \
                           | GET_DST_DRQ(config->slave_id);

        }
        prev = sunxi_lli_list(prev, l_item, chan);
#ifdef DMA_DEBUG
        printf("[sunxi_prep_dma_cyclic]:%p %p %p\n", prev, __va_to_pa(l_item), chan);
#endif
    }
    prev->p_lln = __va_to_pa((uint32_t)chan->desc);
    chan->cyclic = true;
#ifdef DMA_DEBUG
    for (prev = chan->desc; prev != NULL; prev = prev->vlln)
    {
        sunxi_dump_lli(chan, prev);
    }
#endif
    MELIS_CPU_CRITICAL_LEAVE();
    /* hal_spin_unlock_irqrestore(__cpsr); */
    return 0;
}

enum dma_status sunxi_tx_status(struct sunxi_dma_chan *chan, uint32_t *left_size)
{
    uint32_t i = 0;
    struct sunxi_dma_lli *l_item = NULL;
    enum dma_status status = DMA_INVALID_PARAMETER;
    uint32_t __cpsr;

    if (NULL == chan || NULL == left_size)
    {
        return status;
    }

    __cpsr = hal_spin_lock_irqsave();
    if (chan->cyclic)
    {
        for (i = 0, l_item = chan->desc; i <= chan->periods_pos; i++, l_item = l_item->vlln)
        {
            if (NULL == l_item)
            {
                *left_size = 0;
                status = DMA_COMPLETE;
                goto unlock;
            }
        }
        if (NULL == l_item)
        {
            *left_size = 0;
            status = DMA_COMPLETE;
        }
        else
        {
            uint32_t pos = 0;
            bool count = false;

            pos = hal_readl(DMA_LLI_ADDR(chan->chan_count));
            *left_size = hal_readl(DMA_CNT(chan->chan_count));
            if (pos == LINK_END)
            {
                status = DMA_COMPLETE;
                goto unlock;
            }
            for (l_item = chan->desc; l_item != NULL; l_item = l_item->vlln)
            {
                if (l_item->p_lln == pos)
                {
                    count = true;
                    continue;
                }
                if (count)
                {
                    *left_size += l_item->len;
                }
            }
            status = DMA_IN_PROGRESS;
        }
    }
    else
    {
        *left_size = hal_readl(DMA_CNT(chan->chan_count));

        if (*left_size == 0)
        {
            status = DMA_COMPLETE;
        }
        else
        {
            status = DMA_IN_PROGRESS;
        }
    }
unlock:
    hal_spin_unlock_irqrestore(__cpsr);
    return status;
}


int sunxi_dma_start_desc(struct sunxi_dma_chan *chan)
{
    uint32_t high = 0;
    uint32_t irq_val = 0;
    struct sunxi_dma_lli *prev = NULL;
    uint32_t __cpsr;

    if (NULL == chan)
    {
        return -1;
    }

    __cpsr = hal_spin_lock_irqsave();

    if (chan->cyclic)
    {
        chan->irq_type = IRQ_PKG;
    }
    else
    {
        chan->irq_type = IRQ_QUEUE;
    }

    high = (chan->chan_count >= HIGH_CHAN) ? 1 : 0;

    irq_val = hal_readl(DMA_IRQ_EN(high));
    irq_val |= SHIFT_IRQ_MASK(chan->irq_type, chan->chan_count);
    hal_writel(irq_val, DMA_IRQ_EN(high));

    SET_OP_MODE(chan->chan_count, SRC_HS_MASK | DST_HS_MASK);

    for (prev = chan->desc; prev != NULL; prev = prev->vlln)
    {
        cpu_dcache_clean((unsigned long)prev, sizeof(*prev));
        /* k_dcache_clean(prev, sizeof(*prev)); */
        //k_dcache_clean(prev->src, prev->len);
        //k_dcache_clean_invalidate(prev->dst, prev->len);
    }

    /* hal_writel((uint32_t)chan->desc, DMA_LLI_ADDR(chan->chan_count)); */
    hal_writel(__va_to_pa((uint32_t)chan->desc), DMA_LLI_ADDR(chan->chan_count));
    hal_writel(CHAN_START, DMA_ENABLE(chan->chan_count));
    sunxi_dump_com_regs();
    sunxi_dump_chan_regs(chan);
    hal_spin_unlock_irqrestore(__cpsr);
    return 0;
}

int sunxi_dma_stop_desc(struct sunxi_dma_chan *chan)
{
    uint32_t __cpsr;
    if (NULL == chan)
    {
        return -1;
    }

    __cpsr = hal_spin_lock_irqsave();
    /*We should entry PAUSE state first to avoid missing data
    * count witch transferring on bus.
    */
    hal_writel(CHAN_PAUSE, DMA_PAUSE(chan->chan_count));
    hal_writel(CHAN_STOP, DMA_ENABLE(chan->chan_count));
    hal_writel(CHAN_RESUME, DMA_PAUSE(chan->chan_count));

    if (chan->cyclic)
    {
        chan->cyclic = false;
    }
    //chan->desc = NULL;
    hal_spin_unlock_irqrestore(__cpsr);

    return 0;
}
