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

#include <stdint.h>
#include <sunxi_hal_common.h>
#include <sunxi_drv_udc.h>
//#include "drivers/hal_clk.h"
//#include "drivers/hal_gpio.h"

#include "../../../../drivers/hal/source/usb/udc/udc.h"
#include "../../../../drivers/hal/source/ccmu/sun8iw19p1/clk_sun8iw19.h"

/* for test */
//#include "arch/mach/platform.h"

hal_udc_status_t hal_udc_init(void)
{
	uint32_t reg_val;
	uint32_t i;
	/* enable clk */
	//hal_clock_enable();
	//reg_val = hal_readl(SUNXI_CCMU_PBASE + 0x0a8c);
	reg_val = hal_readl(CLK_USB_GATE);
	reg_val |= (0x1 << 24);
	hal_writel(reg_val, CLK_USB_GATE);
	reg_val = hal_readl(CLK_USB_GATE);
	reg_val |= (0x1 << 8);
	hal_writel(reg_val, CLK_USB_GATE);

	reg_val = hal_readl(CLK_USB0_CFG);
	reg_val |= (0x1 << 30);
	hal_writel(reg_val, CLK_USB0_CFG);
	reg_val = hal_readl(CLK_USB0_CFG);
	reg_val |= (0x1 << 29);
	hal_writel(reg_val, CLK_USB0_CFG);

	/* request gpio */
	//hal_pinmux_set_function();

	udc_init();

	return HAL_UDC_STATUS_OK;
}

hal_udc_status_t hal_udc_deinit(void)
{
	udc_deinit();

	/* close clock */
	/* free gpio */

	return HAL_UDC_STATUS_OK;
}

hal_udc_status_t hal_udc_enter_test_mode(uint32_t test_mode)
{
	udc_enter_test_mode(test_mode);

	return HAL_UDC_STATUS_OK;
}

hal_udc_status_t hal_udc_device_desc_init(struct usb_device_descriptor *device_desc)
{
	udc_device_desc_init(device_desc);

	return HAL_UDC_STATUS_OK;
}

hal_udc_status_t hal_udc_config_desc_init(void *config_desc, uint32_t len)
{
	udc_config_desc_init(config_desc, len);

	return HAL_UDC_STATUS_OK;
}

hal_udc_status_t hal_udc_string_desc_init(const void *string_desc)
{
	udc_string_desc_init(string_desc);

	return HAL_UDC_STATUS_OK;
}

hal_udc_status_t hal_udc_register_callback(udc_callback_t user_callback)
{
	udc_register_callback(user_callback);

	return HAL_UDC_STATUS_OK;
}

int32_t hal_udc_ep_read(uint8_t ep_addr, void *buf, uint32_t len)
{
	return udc_ep_read(ep_addr, buf, len);
}

int32_t hal_udc_ep_write(uint8_t ep_addr, void *buf , uint32_t len)
{
	return udc_ep_write(ep_addr, buf, len);
}

hal_udc_status_t hal_udc_ep_enable(uint8_t ep_addr, uint16_t maxpacket, uint32_t ts_type)
{
	udc_ep_enable(ep_addr, maxpacket, ts_type);

	return HAL_UDC_STATUS_OK;
}

hal_udc_status_t hal_udc_ep_disable(uint8_t ep_addr)
{
	udc_ep_disable(ep_addr);

	return HAL_UDC_STATUS_OK;
}

hal_udc_status_t hal_udc_ep_set_buf(uint8_t ep_addr, void *buf, uint32_t len)
{
	udc_ep_set_buf(ep_addr, buf, len);

	return HAL_UDC_STATUS_OK;
}
