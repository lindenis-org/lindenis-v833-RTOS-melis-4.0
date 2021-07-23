/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PART'S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNER'SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PART'S TECHNOLOGY.


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

#ifndef __HAL_UDC_H__
#define __HAL_UDC_H__

#include <stdint.h>

#include "../../drivers/hal/source/usb/udc/udc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	HAL_UDC_STATUS_INVALID_PARAMETER = -2,
	HAL_UDC_STATUS_ERROR             = -1,
	HAL_UDC_STATUS_OK                =  0
} hal_udc_status_t;


hal_udc_status_t hal_udc_init(void);
hal_udc_status_t hal_udc_deinit(void);
hal_udc_status_t hal_udc_enter_test_mode(uint32_t test_mode);
hal_udc_status_t hal_udc_device_desc_init(struct usb_device_descriptor *device_desc);
hal_udc_status_t hal_udc_config_desc_init(void *config_desc, uint32_t len);
hal_udc_status_t hal_udc_string_desc_init(const void *string_desc);
hal_udc_status_t hal_udc_register_callback(udc_callback_t user_callback);
int32_t hal_udc_ep_read(uint8_t ep_addr, void *buf, uint32_t len);
int32_t hal_udc_ep_write(uint8_t ep_addr, void *buf , uint32_t len);
hal_udc_status_t hal_udc_ep_enable(uint8_t ep_addr, uint16_t maxpacket, uint32_t ts_type);
hal_udc_status_t hal_udc_ep_disable(uint8_t ep_addr);
hal_udc_status_t hal_udc_ep_set_buf(uint8_t ep_addr, void *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UDC_H__ */

