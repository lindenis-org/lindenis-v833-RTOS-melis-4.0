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
#include "sunxi_dma_core.h"
#include <sunxi_hal_dma.h>
#include <hal_mem.h>

void dma_free_coherent(void *addr)
{
    void *malloc_ptr = NULL;
    if (!addr)
    {
        return;
    }
    malloc_ptr = (void *)*(uint32_t *)((uint32_t *)addr - 1);
    hal_free(malloc_ptr);
}

void *dma_alloc_coherent(size_t size)
{
    void *fake_ptr = NULL;
    void *malloc_ptr = NULL;

    malloc_ptr = hal_malloc(size + 64);
    if ((uint32_t)malloc_ptr & 0x3)
    {
        printf("error: krhino_mm_alloc not align to 4 byte\r\n");
    }
    fake_ptr = (void *)((uint32_t)(malloc_ptr + 64) & (~63));
    *(uint32_t *)((uint32_t *)fake_ptr - 1) = (uint32_t)malloc_ptr;

    return fake_ptr;
}

hal_dma_chan_status_t hal_dma_chan_request(unsigned long **hdma)
{

    *hdma = (unsigned long *)sunxi_dma_chan_request();

    if (*hdma == 0)
    {
        return HAL_DMA_CHAN_STATUS_BUSY;
    }
    else
    {
        return HAL_DMA_CHAN_STATUS_FREE;
    }
}

hal_dma_status_t hal_dma_prep_memcpy(unsigned long *hdma, uint32_t dest, uint32_t src, uint32_t len)
{
    struct sunxi_dma_chan *chan = NULL;
    int ret;

    if (hdma == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }

    if (dest == 0 || src == 0)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }

    chan = (struct sunxi_dma_chan *)hdma;

    ret = sunxi_prep_dma_memcpy(chan, dest, src, len);

    if (ret == HAL_DMA_STATUS_ERROR)
    {
        return HAL_DMA_STATUS_ERROR;
    }

    return HAL_DMA_STATUS_OK;
}

hal_dma_status_t hal_dma_prep_device(unsigned long *hdma, uint32_t dest, uint32_t src, uint32_t len, enum dma_transfer_direction dir)
{
    struct sunxi_dma_chan *chan = NULL;
    int ret;
    if (hdma == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }
    if (dest == 0 || src == 0)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }
    chan = (struct sunxi_dma_chan *)hdma;
    ret = sunxi_prep_dma_device(chan, dest, src, len, dir);
    if (ret == HAL_DMA_STATUS_ERROR)
    {
        return HAL_DMA_STATUS_ERROR;
    }
    return HAL_DMA_STATUS_OK;
}

hal_dma_status_t hal_dma_prep_cyclic(unsigned long *hdma, uint32_t buf_addr, uint32_t buf_len, uint32_t period_len, enum dma_transfer_direction dir)
{
    struct sunxi_dma_chan *chan = NULL;
    int ret;

    if (hdma == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }

    if (buf_addr == 0)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }

    chan = (struct sunxi_dma_chan *)hdma;

    ret = sunxi_prep_dma_cyclic(chan, buf_addr, buf_len, period_len, dir);

    if (ret == HAL_DMA_STATUS_ERROR)
    {
        return HAL_DMA_STATUS_ERROR;
    }

    return HAL_DMA_STATUS_OK;
}

hal_dma_status_t hal_dma_cyclic_callback_install(unsigned long *hdma,
        dma_callback callback, void *callback_param)
{
    struct sunxi_dma_chan *chan = NULL;
    int ret;
    if (hdma == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }
    if (callback == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }
    if (callback_param == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }
    chan = (struct sunxi_dma_chan *)hdma;
    chan->callback = callback;
    chan->callback_param = callback_param;
    return HAL_DMA_STATUS_OK;
}
hal_dma_status_t hal_dma_slave_config(unsigned long *hdma, struct dma_slave_config *config)
{
    struct sunxi_dma_chan *chan = NULL;

    if (hdma == NULL || config == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }

    chan = (struct sunxi_dma_chan *)hdma;

    dma_slave_config(chan, config);

    return HAL_DMA_STATUS_OK;
}

enum dma_status hal_dma_tx_status(unsigned long *hdma, uint32_t *left_size)
{
    struct sunxi_dma_chan *chan = NULL;

    if (hdma == NULL || left_size == NULL)
    {
        return DMA_INVALID_PARAMETER;
    }

    chan = (struct sunxi_dma_chan *)hdma;

    return sunxi_tx_status(chan, left_size);
}

hal_dma_status_t hal_dma_start(unsigned long *hdma)
{
    struct sunxi_dma_chan *chan = NULL;
    int ret;

    if (hdma == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }

    chan = (struct sunxi_dma_chan *)hdma;

    ret = sunxi_dma_start_desc(chan);

    if (ret != HAL_DMA_STATUS_OK)
    {
        return HAL_DMA_STATUS_ERROR;
    }

    return HAL_DMA_STATUS_OK;
}

hal_dma_status_t hal_dma_stop(unsigned long *hdma)
{
    struct sunxi_dma_chan *chan = NULL;
    int ret;

    if (hdma == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }

    chan = (struct sunxi_dma_chan *)hdma;

    ret = sunxi_dma_stop_desc(chan);

    if (ret != HAL_DMA_STATUS_OK)
    {
        return HAL_DMA_STATUS_ERROR;
    }

    return HAL_DMA_STATUS_OK;
}

hal_dma_status_t hal_dma_chan_free(unsigned long *hdma)
{
    struct sunxi_dma_chan *chan = NULL;
    int ret;

    if (hdma == NULL)
    {
        return HAL_DMA_STATUS_INVALID_PARAMETER;
    }

    chan = (struct sunxi_dma_chan *)hdma;

    ret = sunxi_dma_chan_free(chan);

    if (ret < 0)
    {
        return HAL_DMA_STATUS_ERROR;
    }

    return HAL_DMA_STATUS_OK;
}

void hal_dma_init(void)//only need to be executed once
{
    sunxi_dma_init();
}

