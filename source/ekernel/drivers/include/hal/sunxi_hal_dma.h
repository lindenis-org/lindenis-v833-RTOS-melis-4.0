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
#ifndef __SUNXI_HAL_DMA_H__
#define __SUNXI_HAL_DMA_H__
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* #ifdef  CONFIG_ARCH_SUN8IW19P1 */
#include "sun8i/dma-sun8iw19.h"
#define SUNXI_DMAC_PBASE         0x03002000
/* #endif */

#ifdef __cplusplus
extern "C" {
#endif

#define sunxi_slave_id(d, s) 	(((d)<<16) | (s))

typedef void (*dma_callback)(void *param);

/**
 * enum dma_slave_buswidth - defines bus width of the DMA slave
 * device, source or target buses
 */
enum dma_slave_buswidth {
	DMA_SLAVE_BUSWIDTH_UNDEFINED = 0,
	DMA_SLAVE_BUSWIDTH_1_BYTE = 1,
	DMA_SLAVE_BUSWIDTH_2_BYTES = 2,
	DMA_SLAVE_BUSWIDTH_3_BYTES = 3,
	DMA_SLAVE_BUSWIDTH_4_BYTES = 4,
	DMA_SLAVE_BUSWIDTH_8_BYTES = 8,
	DMA_SLAVE_BUSWIDTH_16_BYTES = 16,
	DMA_SLAVE_BUSWIDTH_32_BYTES = 32,
	DMA_SLAVE_BUSWIDTH_64_BYTES = 64,
};

enum dma_slave_burst {
	DMA_SLAVE_BURST_1 = 1,
	DMA_SLAVE_BURST_4 = 4,
	DMA_SLAVE_BURST_8 = 8,
	DMA_SLAVE_BURST_16 = 16,
};

/**
 * enum dma_transfer_direction - dma transfer mode and direction indicator
 * @DMA_MEM_TO_MEM: Async/Memcpy mode
 * @DMA_MEM_TO_DEV: Slave mode & From Memory to Device
 * @DMA_DEV_TO_MEM: Slave mode & From Device to Memory
 * @DMA_DEV_TO_DEV: Slave mode & From Device to Device
 */
enum dma_transfer_direction {
	DMA_MEM_TO_MEM = 0,
	DMA_MEM_TO_DEV = 1,
	DMA_DEV_TO_MEM = 2,
	DMA_DEV_TO_DEV = 3,
	DMA_TRANS_NONE,
};

/**
 * enum dma_status - DMA transaction status
 * @DMA_COMPLETE: transaction completed
 * @DMA_IN_PROGRESS: transaction not yet processed
 * @DMA_PAUSED: transaction is paused
 * @DMA_ERROR: transaction failed
 */
enum dma_status {
	DMA_INVALID_PARAMETER = -2,
	DMA_ERROR = -1,
	DMA_COMPLETE,
	DMA_IN_PROGRESS,
	DMA_PAUSED,
};

/**
 * struct dma_slave_config - dma slave channel runtime config
 * @direction: whether the data shall go in or out on this slave
 * channel, right now. DMA_MEM_TO_DEV and DMA_DEV_TO_MEM are
 * legal values. DEPRECATED, drivers should use the direction argument
 * to the device_prep_slave_sg and device_prep_dma_cyclic functions or
 * the dir field in the dma_interleaved_template structure.
 * @src_addr: this is the physical address where DMA slave data
 * should be read (RX), if the source is memory this argument is
 * ignored.
 * @dst_addr: this is the physical address where DMA slave data
 * should be written (TX), if the source is memory this argument
 * is ignored.
 * @src_addr_width: this is the width in bytes of the source (RX)
 * register where DMA data shall be read. If the source
 * is memory this may be ignored depending on architecture.
 * Legal values: 1, 2, 4, 8.
 * @dst_addr_width: same as src_addr_width but for destination
 * target (TX) mutatis mutandis.
 * @src_maxburst: the maximum number of words (note: words, as in
 * units of the src_addr_width member, not bytes) that can be sent
 * in one burst to the device. Typically something like half the
 * FIFO depth on I/O peripherals so you don't overflow it. This
 * may or may not be applicable on memory sources.
 * @dst_maxburst: same as src_maxburst but for destination target
 * mutatis mutandis.
 * @slave_id: Slave requester id. Only valid for slave channels. The dma
 * slave peripheral will have unique id as dma requester which need to be
 * pass as slave config.
 *
 * This struct is passed in as configuration data to a DMA engine
 * in order to set up a certain channel for DMA transport at runtime.
 * The DMA device/engine has to provide support for an additional
 * callback in the dma_device structure, device_config and this struct
 * will then be passed in as an argument to the function.
 *
 * The rationale for adding configuration information to this struct is as
 * follows: if it is likely that more than one DMA slave controllers in
 * the world will support the configuration option, then make it generic.
 * If not: if it is fixed so that it be sent in static from the platform
 * data, then prefer to do that.
 */
struct dma_slave_config {
	enum dma_transfer_direction direction;
	uint32_t src_addr;
	uint32_t dst_addr;
	enum dma_slave_buswidth src_addr_width;
	enum dma_slave_buswidth dst_addr_width;
	uint32_t src_maxburst;
	uint32_t dst_maxburst;
	uint32_t slave_id;
};

struct sunxi_dma_chan {
	uint8_t used:1;
	uint8_t chan_count:4;
	bool	cyclic:1;
	struct dma_slave_config  cfg;
	uint32_t periods_pos;
	uint32_t buf_len;
	struct sunxi_dma_lli *desc;
	uint32_t	irq_type;
	dma_callback callback;
	void *callback_param;
	/* volatile kspinlock_t lock; */
	volatile int lock;
};

/** This enum defines the DMA CHANNEL status. */
typedef enum {
	HAL_DMA_CHAN_STATUS_BUSY  = 0,              /* DMA channel status busy */
	HAL_DMA_CHAN_STATUS_FREE = 1               /* DMA channel status free */
} hal_dma_chan_status_t;

/** This enum defines the return type of GPIO API. */
typedef enum {
	HAL_DMA_STATUS_INVALID_PARAMETER         = -2,     /**< Invalid status. */
	HAL_DMA_STATUS_ERROR         = -1,     /**< Invalid input parameter. */
	HAL_DMA_STATUS_OK                = 0       /**< The DMA status ok. */
} hal_dma_status_t;


hal_dma_chan_status_t hal_dma_chan_request(unsigned long **hdma);
hal_dma_status_t hal_dma_prep_cyclic(unsigned long *hdma, uint32_t buf_addr, uint32_t buf_len, uint32_t period_len, enum dma_transfer_direction dir);
hal_dma_status_t hal_dma_cyclic_callback_install(unsigned long *hdma, dma_callback callback, void *callback_param);
hal_dma_status_t hal_dma_prep_memcpy(unsigned long *hdma, uint32_t dest, uint32_t src, uint32_t len);
hal_dma_status_t hal_dma_prep_device(unsigned long *hdma, uint32_t dest, uint32_t src, uint32_t len, enum dma_transfer_direction dir);
hal_dma_status_t hal_dma_slave_config(unsigned long *hdma, struct dma_slave_config*config);
enum dma_status hal_dma_tx_status(unsigned long *hdma, uint32_t *left_size);
hal_dma_status_t hal_dma_start(unsigned long *hdma);
hal_dma_status_t hal_dma_stop(unsigned long *hdma);
hal_dma_status_t hal_dma_chan_free(unsigned long *hdma);
void * dma_map_area(void * addr, size_t size, enum dma_transfer_direction dir);
void dma_unmap_area(void * addr, size_t size, enum dma_transfer_direction dir);

void hal_dma_init(void);
void dma_free_coherent(void *addr);
void *dma_alloc_coherent(size_t size);

#ifdef __cplusplus
}
#endif

#endif
