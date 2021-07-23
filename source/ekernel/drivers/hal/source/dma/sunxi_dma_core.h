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

#ifndef __SUNXI_DMA_CORE_H__
#define __SUNXI_DMA_CORE_H__

#include <sunxi_hal_dma.h>
#include <stdio.h>
#include "sunxi_hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* #define DMA_DEBUG */

#define HEXADECIMAL (0x10)
#define REG_INTERVAL (0x04)
#define REG_CL (0x0c)

#define NR_MAX_CHAN		8				/* total of channels */

#define HIGH_CHAN		8

#define DMA_IRQ_EN(x)		(SUNXI_DMAC_PBASE + (0x00 + ((x) << 2)))		/* Interrupt enable register */
#define DMA_IRQ_STAT(x)		(SUNXI_DMAC_PBASE + (0x10 + ((x) << 2)))		/* Interrupt status register */
#define DMA_SECURE		(SUNXI_DMAC_PBASE + 0x20)				/* DMA security register */
#define DMA_GATE		(SUNXI_DMAC_PBASE + 0x28)				/* DMA gating register */
#define DMA_MCLK_GATE		0x04
#define DMA_COMMON_GATE		0x02
#define DMA_CHAN_GATE		0x01
#define DMA_STAT		(SUNXI_DMAC_PBASE + 0x30)			/* DMA Status Register RO */
#define DMA_ENABLE(x)		(SUNXI_DMAC_PBASE + (0x100 + ((x) << 6)))	/* Channels enable register */
#define DMA_PAUSE(x)		(SUNXI_DMAC_PBASE + (0x104 + ((x) << 6)))	/* DMA Channels pause register */
#define DMA_LLI_ADDR(x)		(SUNXI_DMAC_PBASE + (0x108 + ((x) << 6)))	/* Descriptor address register */
#define DMA_CFG(x)				(SUNXI_DMAC_PBASE + 0x10C + ((x) << 6))	/* Configuration register RO */
#define DMA_CUR_SRC(x)		(SUNXI_DMAC_PBASE + (0x110 + ((x) << 6)))	/* Current source address RO */
#define DMA_CUR_DST(x)		(SUNXI_DMAC_PBASE + (0x114 + ((x) << 6)))	/* Current destination address RO */
#define DMA_CNT(x)		(SUNXI_DMAC_PBASE + (0x118 + ((x) << 6)))	/* Byte counter left register RO */
#define DMA_PARA(x)		(SUNXI_DMAC_PBASE + (0x11C + ((x) << 6)))	/* Parameter register RO */
#define LINK_END		0xFFFFF800			/* lastest link must be 0xfffff800 */

	/* DMA mode register */
#define DMA_OP_MODE(x)		(SUNXI_DMAC_PBASE + (0x128 + ((x) << 6)))		/* DMA mode register */
#define SRC_HS_MASK		(0x1 << 2)			/* bit 2: Source handshake mode */
#define DST_HS_MASK		(0x1 << 3)			/* bit 3: Destination handshake mode */

#define SET_OP_MODE(x, val)	({	\
		writel(val,DMA_OP_MODE(x));	\
		})


#define SHIFT_IRQ_MASK(val, ch) ({	\
		(ch) >= HIGH_CHAN	\
		? (val) << ((ch - HIGH_CHAN) << 2) \
		: (val) << ((ch) << 2);	\
		})

#define SHIFT_PENDING_MASK(val, ch) ({	\
		(ch) >= HIGH_CHAN	\
		? (val) << ((ch - HIGH_CHAN) << 2) \
		: (val) << ((ch) << 2);	\
		})

#define IRQ_HALF		0x01			/* Half package transfer interrupt pending */
#define IRQ_PKG			0x02			/* One package complete interrupt pending */
#define IRQ_QUEUE		0x04			/* All list complete transfer interrupt pending */

	/* DMA channel configuration register */
	/* The detail information of DMA configuration */
#define SRC_WIDTH(x)		((x) << 9)
#define SRC_BURST(x)		((x) << 6)
#define SRC_IO_MODE		(0x01 << 8)
#define SRC_LINEAR_MODE		(0x00 << 8)
#define SRC_DRQ(x)		((x) << 0)
#define DST_WIDTH(x)		((x) << 25)
#define DST_BURST(x)		((x) << 22)
#define DST_IO_MODE		(0x01 << 24)
#define DST_LINEAR_MODE		(0x00 << 24)
#define DST_DRQ(x)		((x) << 16)
#define CHAN_START		1
#define CHAN_STOP		0
#define CHAN_PAUSE		1
#define CHAN_RESUME		0
#define NORMAL_WAIT		(8 << 0)

#define GET_SRC_DRQ(x)		((x) & 0x000000ff)
#define GET_DST_DRQ(x)		((x) & 0x00ff0000)


struct sunxi_dma_lli {
	uint32_t	cfg;
	uint32_t	src;
	uint32_t	dst;
	uint32_t	len;
	uint32_t	para;
	uint32_t	p_lln;
	struct sunxi_dma_lli *vlln;
};

struct sunxi_dma_chan_config {
	uint32_t	enable;
	uint32_t	pause;
	uint32_t	desc_addr;
	uint32_t	config;
	uint32_t	src_addr;
	uint32_t	dst_addr;
	uint32_t	left_byte;
	uint32_t	parameters;
};

void sunxi_dma_init(void);
unsigned long sunxi_dma_chan_request(void);
int sunxi_dma_chan_free(struct sunxi_dma_chan *chan);
int dma_slave_config(struct sunxi_dma_chan *chan, struct dma_slave_config*config);
int sunxi_prep_dma_memcpy(struct sunxi_dma_chan * chan, uint32_t dest, uint32_t src, uint32_t len);
int sunxi_prep_dma_device(struct sunxi_dma_chan * chan, uint32_t dest, uint32_t src, uint32_t len, enum dma_transfer_direction dir);
int sunxi_prep_dma_cyclic(struct sunxi_dma_chan * chan, uint32_t buf_addr, uint32_t buf_len, uint32_t period_len, enum dma_transfer_direction dir);
int sunxi_dma_start_desc(struct sunxi_dma_chan *chan);
int sunxi_dma_stop_desc(struct sunxi_dma_chan *chan);

enum dma_status sunxi_tx_status(struct sunxi_dma_chan *chan, uint32_t *left_size);
#ifdef __cplusplus
}
#endif

#endif
