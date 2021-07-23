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

#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>
#include <interrupt.h>
#include <hal_cache.h>
#include <hal_mem.h>
#include <sunxi_drv_crypto.h>
#include "ce_reg.h"
#include "hal_ce_common.h"
#include "waitqueue.h"

//#define CE_NO_IRQ
#define IRQ_DONE		(0x5A5A)
#define CE_WAIT_TIME	(50000)

extern void udelay(rt_uint32_t us);

static rt_uint32_t irq_done;
static rt_wqueue_t ce_wqueue;

void ce_print_hex(char *_data, int _len, void *_addr)
{
	int i;

	CE_DBG("---------------- The valid len = %d ----------------\n",
		_len);
	for (i = 0; i < _len/8; i++) {
		CE_DBG("0x%p: %02X %02X %02X %02X %02X %02X %02X %02X\n",
			i*8 + _addr,
			_data[i*8+0], _data[i*8+1], _data[i*8+2], _data[i*8+3],
			_data[i*8+4], _data[i*8+5], _data[i*8+6], _data[i*8+7]);
	}
	CE_DBG("----------------------------------------------------\n");
}

void ce_print_task_info(ce_task_desc_t *task)
{
	CE_DBG("-----------task_info------\n");
	CE_DBG("task = 0x%x\n", (uint32_t)task);
	CE_DBG("task->comm_ctl = 0x%lx\n", task->comm_ctl);
	CE_DBG("task->sym_ctl = 0x%lx\n", task->sym_ctl);
	CE_DBG("task->asym_ctl = 0x%lx\n", task->asym_ctl);
	CE_DBG("task->chan_id = 0x%lx\n", task->chan_id);
	CE_DBG("task->ctr_addr = 0x%lx\n", task->ctr_addr);
	CE_DBG("task->data_len = 0x%lx\n", task->data_len);
	CE_DBG("task->iv_addr = 0x%lx\n", task->iv_addr);
	CE_DBG("task->key_addr = 0x%lx\n", task->key_addr);
	CE_DBG("task->src[0].addr = 0x%lx\n", task->src[0].addr);
	CE_DBG("task->src[0].len = 0x%lx\n", task->src[0].len);
	CE_DBG("task->dst[0].addr = 0x%lx\n", task->dst[0].addr);
	CE_DBG("task->dst[0].len = 0x%lx\n", task->dst[0].len);
}


int wait_for_completion_timeout(rt_uint32_t time)
{
	rt_uint32_t counter = 0;
	irq_done  = 0;

	while(counter <= time) {
		if (irq_done == IRQ_DONE) {
			return 0;
		}
		udelay(1000);
		counter ++;
	}
	return -1;
}

static irqreturn_t ce_irq_handler(int irq, void *dev_id)
{
	int i;
	int pending = 0;

	pending = ce_pending_get();
	for (i = 0; i < CE_FLOW_NUM; i++) {
		if (pending & (CE_CHAN_PENDING << i)) {
			CE_DBG("Chan %d completed. pending: %#x\n", i, pending);
			ce_pending_clear(i);
			printf("%s line %d, irq done.\n", __func__, __LINE__);
			irq_done = IRQ_DONE;
			rt_wqueue_wakeup(&ce_wqueue, NULL);
		}
	}

	return IRQ_HANDLED;
}


static int ce_irq_request(void)
{
	rt_uint32_t irqn = SUNXI_IRQ_CE;

	if (request_irq(irqn, ce_irq_handler, 0, "crypto", NULL) < 0) {
		CE_ERR("Cannot request IRQ\n");
		return -1;
	}

	enable_irq(irqn);

	return 0;
}

int sunxi_ce_init(void)
{
	int ret = 0;

	ce_clock_init();
	ret = ce_irq_request();
	if (ret < 0) {
		return -1;
	}

	irq_done = 0x0;
	rt_wqueue_init(&ce_wqueue);

	return 0;
}

static void ce_task_desc_init(ce_task_desc_t *task, rt_uint32_t flow)
{
	memset((void *)task, 0x0, sizeof(ce_task_desc_t));
	task->chan_id = flow;
	task->comm_ctl |= CE_COMM_CTL_TASK_INT_MASK;
}

static int aes_crypto_start(rt_uint8_t *src_buf,
                          rt_uint32_t src_length,
                          rt_uint8_t *dest_buf,
                          ce_task_desc_t *task)
{
	int ret = 0;
	int data_word_len = 0;
	
	ce_pending_clear(task->chan_id);

	if ((src_length & 0x3) == 0) {
		data_word_len = src_length >> 2;
	} else {
		data_word_len = (src_length >> 2) + 1;
	}
#ifdef SS_SUPPORT_CE_V3_1
	ce_data_len_set((src_length >> 2), task);
#else
	ce_data_len_set(src_length, task);
#endif
	task->src[0].addr = (rt_uint32_t)__va_to_pa((rt_uint32_t)src_buf);
	task->src[0].len = data_word_len;
	
	task->dst[0].addr = (rt_uint32_t)__va_to_pa((rt_uint32_t)dest_buf);
	task->dst[0].len = data_word_len;
	task->next = 0;
	cpu_dcache_clean((rt_uint32_t)task, sizeof(ce_task_desc_t));
	cpu_dcache_clean((rt_uint32_t)src_buf, src_length);
	cpu_dcache_clean((rt_uint32_t)dest_buf, src_length);
	void dma_map_area(rt_uint32_t start, rt_uint32_t size, rt_int32_t dir);
	void dma_unmap_area(rt_uint32_t start, rt_uint32_t size, rt_int32_t dir);
	/*ce_print_task_info(task);*/
	dma_map_area((rt_uint32_t)dest_buf, src_length, DMA_FROM_DEVICE);
	ce_set_tsk((rt_uint32_t)task);
	ce_irq_enable(task->chan_id);
	ce_ctrl_start();

#ifdef CE_NO_IRQ
	ce_wait_finish(task->chan_id);
#else
#if 0
	ret = wait_for_completion_timeout(CE_WAIT_TIME);
	if (ret != 0) {
		CE_ERR("Timed out\n");
		ret = AES_TIME_OUT;
	}
#endif
	rt_wqueue_wait(&ce_wqueue, 0, RT_WAITING_FOREVER);
#endif
	dma_unmap_area((rt_uint32_t)dest_buf, src_length, DMA_FROM_DEVICE);
	cpu_dcache_invalidate((rt_uint32_t)task, sizeof(ce_task_desc_t));
	/*cpu_dcache_invalidate((rt_uint32_t)dest_buf, src_length);*/
#if 0
	ce_print_hex((char *)task->dst[0].addr, (task->dst[0].len * 4), (char *)task->dst[0].addr);
	ce_print_hex((char *)task->src[0].addr, (task->src[0].len * 4), (char *)task->src[0].addr);
	ce_print_hex((char *)task->iv_addr, (16), (char *)task->iv_addr);
	ce_print_hex((char *)task->key_addr, (16), (char *)task->key_addr);
#endif

	ce_irq_disable(task->chan_id);
	if (ce_get_erro() > 0) {
		ce_reg_printf();
		ret = AES_TIME_OUT;
	}


	return AES_STATUS_OK;
}

static ce_task_desc_t *ce_aes_config(rt_uint8_t dir, rt_uint8_t type, rt_uint8_t mode, rt_uint8_t *key_buf, rt_uint32_t key_length)
{
	ce_task_desc_t *task = NULL;
	rt_uint32_t flow = 1;

	task = (ce_task_desc_t *)rt_malloc(sizeof(ce_task_desc_t));
	if (task == NULL) {
		CE_ERR("rt_malloc_align fail\n");
		return NULL;
	}
	CE_DBG("task addr = 0x%lx\n", (rt_uint32_t)task);
	ce_task_desc_init(task, flow);
	ce_method_set(dir, type, task);
	ce_aes_mode_set(mode, task);
	ce_key_set(key_buf, key_length, task);
	cpu_dcache_clean((rt_uint32_t)key_buf, key_length);
	return task;
}

int do_aes_crypto(crypto_aes_req_ctx_t *req_ctx)
{
	rt_uint32_t last_block_size = 0;
	rt_uint32_t block_num = 0;
	rt_uint32_t padding_size = 0;
	rt_uint32_t first_encypt_size = 0;
	rt_uint8_t data_block[AES_BLOCK_SIZE] = {0};
	rt_uint8_t *iv;
	rt_uint8_t *init_vector2;
	ce_task_desc_t *task;

	/*ce config*/
	task = ce_aes_config(req_ctx->dir, CE_METHOD_AES, req_ctx->mode, req_ctx->key, req_ctx->key_length);
	if (task == NULL) {
		CE_ERR("ce_aes_config fail\n");
		return AES_INPUT_ERROR;
	}

	if (req_ctx->dir == CRYPTO_DIR_DECRYPT) {
		ce_iv_set(req_ctx->iv, 0, task);
		cpu_dcache_clean((rt_uint32_t)req_ctx->iv, 16);
		if (AES_STATUS_OK != aes_crypto_start(req_ctx->src_buffer,
												req_ctx->src_length,
												req_ctx->dst_buffer,
												task)) {
			CE_ERR("aes decrypt fail\n");
			rt_free(task);
			return AES_CRYPTO_ERROR;
		}

		req_ctx->dst_length = req_ctx->src_length;
		rt_free(task);
		return AES_STATUS_OK;

	} else {
		block_num = req_ctx->src_length / AES_BLOCK_SIZE;
		last_block_size = req_ctx->src_length % AES_BLOCK_SIZE;
		padding_size = AES_BLOCK_SIZE - last_block_size;

		if (block_num > 0) {
			CE_DBG("block_num = %ld\n", block_num);
			first_encypt_size = block_num * AES_BLOCK_SIZE;
			ce_iv_set(req_ctx->iv, 0, task);
			cpu_dcache_clean((rt_uint32_t)req_ctx->iv, 16);
			if (AES_STATUS_OK != aes_crypto_start(req_ctx->src_buffer,
												req_ctx->src_length,
												req_ctx->dst_buffer,
												task)) {
				CE_ERR("do_aes_encrypt fail.\n");
				rt_free(task);
				return AES_CRYPTO_ERROR;
			 }
			req_ctx->dst_length = block_num * AES_BLOCK_SIZE;

			if (last_block_size) {
				CE_DBG("last_block_size = %ld\n", last_block_size);
				CE_DBG("padding_size = %ld\n", padding_size);
				memcpy(data_block, req_ctx->src_buffer + first_encypt_size, last_block_size);
				memset(data_block + last_block_size, padding_size, padding_size);
				if (AES_TYPE_CBC == req_ctx->mode) {
					iv = req_ctx->dst_buffer + first_encypt_size - AES_BLOCK_SIZE;
				} else {
					iv = req_ctx->iv;
				}
				ce_iv_set(iv, 0, task);
				cpu_dcache_clean((rt_uint32_t)req_ctx->iv, 16);

				if (AES_STATUS_OK != aes_crypto_start(data_block,
														AES_BLOCK_SIZE,
														req_ctx->dst_buffer + first_encypt_size,
														task)) {
					CE_ERR("do_aes_encrypt fail\n");
					rt_free(task);
					return AES_CRYPTO_ERROR;
				}
				req_ctx->dst_length = req_ctx->dst_length + AES_BLOCK_SIZE;
			}
		} else {
			CE_DBG("padding_size = %ld\n", padding_size);
			memcpy(data_block, req_ctx->src_buffer, req_ctx->src_length);
			memset(data_block + last_block_size, padding_size, padding_size);
			ce_iv_set(req_ctx->iv, 0, task);
			if (AES_STATUS_OK != aes_crypto_start(data_block,
													AES_BLOCK_SIZE,
													req_ctx->dst_buffer,
													task)) {
				CE_ERR("do_aes_encrypt fail\n");
				return AES_CRYPTO_ERROR;
			}
			req_ctx->dst_length = (block_num + 1) * AES_BLOCK_SIZE;
		}

		CE_DBG("dest_text->length = %ld\n", req_ctx->dst_length);
		rt_free(task);
		return AES_STATUS_OK;
	}
}
