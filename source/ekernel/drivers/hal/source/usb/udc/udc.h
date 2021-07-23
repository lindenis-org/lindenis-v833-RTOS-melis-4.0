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

#ifndef __UDC_H__
#define __UDC_H__

#include <../../../../drivers/include/hal/usb/ch9.h>

int printk(const char *fmt, ...);
/* #define UDC_LOG_DEBUG*/
#define log_udc_info(fmt, ...)	printk("udc: "fmt, ##__VA_ARGS__)
#define log_udc_err(fmt, ...)	printk("udc: "fmt, ##__VA_ARGS__)
#ifdef UDC_LOG_DEBUG
#define log_udc_dbg(fmt, ...)	printk("[%s %d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define log_udc_dbg(fmt, ...)
#endif

#define UDC_MAX_NUM_EP_TX		4
#define UDC_MAX_NUM_EP_RX		4
#define UDC_MAX_PACKET_SIZE_EP0		64
#define UDC_MAX_PACKET_SIZE_EP_BULK	512
#define UDC_MAX_PACKET_SIZE_EP_ISO	1024
#define UDC_MAX_PACKET_SIZE_EP_INT	512
#define UDC_MAX_NUM_STRING_DESC		12

typedef enum {
	UDC_EVENT_RX_STANDARD_REQUEST = 1,
	UDC_EVENT_RX_CLASS_REQUEST = 2,
	UDC_EVENT_RX_DATA = 3,
	UDC_EVENT_TX_COMPLETE = 4,
} udc_callback_event_t;

typedef enum {
	UDC_ERRNO_SUCCESS = 0,
	UDC_ERRNO_CMD_NOT_SUPPORTED = -1,
	UDC_ERRNO_CMD_INVALID = -2,
	UDC_ERRNO_BUF_NULL = -3,
	UDC_ERRNO_BUF_FULL = -4,
	UDC_ERRNO_EP_INVALID = -5,
	UDC_ERRNO_RX_NOT_READY = -6,
	UDC_ERRNO_TX_BUSY = -7,
} udc_errno_t;

typedef udc_errno_t (*udc_callback_t)(uint8_t ep_addr, udc_callback_event_t event,
				void *data, uint32_t len);

typedef enum {
	UDC_IO_TYPE_PIO = 0,
	UDC_IO_TYPE_DMA,
} udc_io_type_t;

typedef enum {
	UDC_EP_TYPE_EP0 = 0,
	UDC_EP_TYPE_TX,
	UDC_EP_TYPE_RX,
} udc_ep_type_t;

typedef enum {
	UDC_EP0_IDLE = 0,
	UDC_EP0_IN_DATA_PHASE,
	UDC_EP0_OUT_DATA_PHASE,
	UDC_EP0_END_XFER,
	UDC_EP0_STALL,
} udc_ep0_state_t;

typedef enum {
	UDC_SPEED_UNKNOWN = 0,		/* enumerating */
	UDC_SPEED_LOW, UDC_SPEED_FULL,	/* usb 1.1 */
	UDC_SPEED_HIGH,			/* usb 2.0 */
	UDC_SPEED_WIRELESS,		/* wireless (usb 2.5) */
	UDC_SPEED_SUPER,		/* usb 3.0 */
	UDC_SPEED_SUPER_PLUS,		/* usb 3.1 */
} udc_speed_t;

typedef struct {
	uint8_t ep_addr;
	uint32_t fifo_addr;
	uint32_t fifo_size;
	uint8_t double_fifo;
} udc_fifo_t;

typedef struct {
	uint8_t				ep_addr;
	void				*pdata;
	uint32_t			data_len;
	uint32_t			data_actual;
	uint32_t			maxpacket;
} udc_ep_t;

typedef struct {
	uint8_t				ep0state;
	uint8_t				address;
	udc_speed_t			speed;

	struct usb_ctrlrequest		crq;
	uint8_t				req_std;
	struct usb_device_descriptor	*device_desc;
	void				*config_desc;
	uint32_t			config_desc_len;
	struct usb_string_descriptor	*string_desc[UDC_MAX_NUM_STRING_DESC];
	uint32_t			string_desc_num;

	udc_ep_t			ep0;
	udc_ep_t			epin[UDC_MAX_NUM_EP_TX];
	udc_ep_t			epout[UDC_MAX_NUM_EP_RX];

	udc_callback_t			callback;
} udc_priv_t;

void udc_device_desc_init(struct usb_device_descriptor *device_desc);
void udc_config_desc_init(void *config_desc, uint32_t len);
void udc_string_desc_init(const void *string_desc);
int32_t udc_ep_read(uint8_t ep_addr, void *buf, uint32_t len);
int32_t udc_ep_write(uint8_t ep_addr, void *buf, uint32_t len);
void udc_ep_enable(uint8_t ep_addr, uint16_t maxpacket, uint32_t ts_type);
void udc_ep_disable(uint8_t ep_addr);
void udc_ep_set_buf(uint8_t ep_addr, void *buf, uint32_t len);
void udc_register_callback(udc_callback_t user_callback);
int32_t udc_enter_test_mode(uint32_t test_mode);
int32_t udc_deinit(void);
int32_t udc_init(void);

#endif /*__UDC_H__*/
