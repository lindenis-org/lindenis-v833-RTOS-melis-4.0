/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
 *
 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 *the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.
 *
 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
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

#ifndef __SUNXI_HAL_CLK_H__
#define __SUNXI_HAL_CLK_H__

#include "typedef.h"
#include <stdio.h>
#include <string.h>


#define hal_clk_api_version "hal_clk_api_version_1_0_2"

/************************************************************************************************
* Macro definition CCMU_XXX
* @Description: These definitions used to CCMU HAL-API and Drivers source code debug
*************************************************************************************************/
//#define CCMU_DBG_EN 1

#if defined(CCMU_DBG_LEAVE_TINY) || defined(CCMU_DBG_LEAVE_HIGH)
#define CCMU_DBG(fmt,args...)       printf("[CCMU:dbg..] %-*s:%d "fmt ,30, __func__, __LINE__, ##args)
#define CCMU_ERR(fmt,args...)       printf("[CCMU:err**] %-*s:%d "fmt ,30, __func__, __LINE__, ##args)
#else
#define CCMU_DBG(fmt,args...)   do{} while(0)
#define CCMU_ERR(fmt,args...)   do{} while(0)
#endif
#if defined(CCMU_DBG_LEAVE_HIGH)
#define CCMU_TRACE()                printf("[CCMU:trace] %-*s:%d \n",30, __func__, __LINE__)
#define CCMU_TRACE_CLK(tpye, clk)   printf("CCMU:trace %s:%d CLK "#tpye" id %d\n",__func__, __LINE__, clk)
#else
#define CCMU_TRACE()            do{} while(0)
#define CCMU_TRACE_CLK(clk, rate)   do{} while(0)
#endif


/************************************************************************************************
* Macro definition readl 、writel hal_read_xxx and hal_write_xxx
* @Description:  These definitions used to CCMU Drivers to read and write Physical I/O register
*************************************************************************************************/
#define hal_write_reg8(addr ,data)     ((*(volatile u8 *)(addr)) = (u8)(data))
#define hal_write_reg16(addr ,data)     ((*(volatile u16 *)(addr)) = (u16)(data))
#define hal_write_reg32(addr ,data)     ((*(volatile u32 *)(addr)) = (u32)(data))
#define hal_read_reg8(x)                (*(volatile u8 *)(x))
#define hal_read_reg16(x)                (*(volatile u16 *)(x))
#define hal_read_reg32(x)                (*(volatile u32 *)(x))

#undef readl
#undef writel
#define readl(x) hal_read_reg32(x)
#define writel(val, reg) hal_write_reg32(reg, val)


/************************************************************************************************
* Enum hal_clk_status_t
* @Description: This enum defines the return status of Clock APIs. User should check the return value after calling the APIs
*************************************************************************************************/

typedef enum
{

    HAL_CLK_STATUS_DISABLED = -1,
    HAL_CLK_STATUS_ENABLED = 0,
    HAL_CLK_STATUS_ERROR_CLK_FACTOR_REFUSED = -11,
    HAL_CLK_STATUS_ERROR_CLK_NEED_DISABLED  = -10,
    HAL_CLK_STATUS_ERROR_CLK_PARENT_DISABLED  = -9,
    HAL_CLK_STATUS_ERROR_CLK_ENABLED_FAILED  = -8,
    HAL_CLK_STATUS_ERROR_CLK_ROUND_FAILED = -7,
    HAL_CLK_STATUS_ERROR_CLK_SET_RATE_REFUSED = -6,
    HAL_CLK_STATUS_ERROR_CLK_NOT_FOUND  = -5,
    HAL_CLK_STATUS_ERROT_CLK_UNDEFINED  = -4,
    HAL_CLK_STATUS_UNINITIALIZED = -3,        /**< Uninitialized clock driver. */
    HAL_CLK_STATUS_INVALID_PARAMETER = -2,    /**< Invalid parameter. */
    HAL_CLK_STATUS_ERROR = -1,                /**< Unknown error. */
    HAL_CLK_STATUS_OK = 0,                    /**< Successful. */
} hal_clk_status_t;

/************************************************************************************************
* Enum hal_clk_type_t
* @Description: This enum defines the type of Clock
*************************************************************************************************/
typedef enum
{
    HAL_CLK_ROOT     = -1,
    HAL_CLK_FIXED_SRC,
    HAL_CLK_FIXED_FACTOR,
    HAL_CLK_FACTOR,
    HAL_CLK_PERIPH,
} hal_clk_type_t;

#define  HAL_CLK_GET_RATE_NOCACHE       (ture)
#define  HAL_CLK_GET_RATE_CACHE         (false)
#define  HAL_CLK_RATE_UNINITIALIZED     (0)
#define  HAL_CLK_RATE_ROOT      (0)

/************************************************************************************************
* Enum hal_clk_id_t
* @Description: This enum defines the Clock-id that CCMU HAL support
* # Clock-id defines as Bitmap 32bits
* #|PERIPH CLOCK   |FACTOR PLL CLOCK  |FIXED FACTOR CLOCK     |FIXED SOURCE CLOCK  |
* #|768~1023       |512~767           |256~511                |0~255               |
*************************************************************************************************/
#define HAL_CLK_SECTION                 (256)
#define HAL_CLK_FIXED_SRC_OFFSET        (HAL_CLK_FIXED_SRC*HAL_CLK_SECTION)
#define HAL_CLK_FIXED_FACTOR_OFFSET (HAL_CLK_FIXED_FACTOR*HAL_CLK_SECTION)
#define HAL_CLK_FACTOR_OFFSET           (HAL_CLK_FACTOR*HAL_CLK_SECTION)
#define HAL_CLK_PERIPH_OFFSET           (HAL_CLK_PERIPH*HAL_CLK_SECTION)
#define HAL_CLK_GET_TYPE(clk_id)        (clk_id/HAL_CLK_SECTION)

typedef enum
{
    HAL_CLK_UNINITIALIZED       = -1,
    HAL_CLK_SRC_ROOT,
    /*
    *   FIXED SOURCE CLOCK 0~255
    */
    HAL_CLK_SRC_HOSC24M     = HAL_CLK_FIXED_SRC_OFFSET,
    HAL_CLK_SRC_HOSC24MD2,
    HAL_CLK_SRC_IOSC16M,
    HAL_CLK_SRC_OSC48M,
    HAL_CLK_SRC_OSC48MD4,
    HAL_CLK_SRC_LOSC,
    HAL_CLK_SRC_RC16M,
    HAL_CLK_PLL_PERI0DIV25M,
    /*
    *   FIXED FACTOR CLOCK 256~511
    */
    HAL_CLK_PLL_PERI0X2         = HAL_CLK_FIXED_FACTOR_OFFSET,
    HAL_CLK_PLL_PERI1X2,
    HAL_CLK_PLL_AUDIOX2,
    HAL_CLK_PLL_AUDIOX4,
    HAL_CLK_PLL_VIDEOX4,
    HAL_CLK_PLL_DDRDIV4,
    /*
    *   FACTOR CLOCK 512~1023
    */
    HAL_CLK_PLL_CPUX_C0     =  HAL_CLK_FACTOR_OFFSET,
    HAL_CLK_PLL_CPUX_C1,
    HAL_CLK_PLL_DDR0,
    HAL_CLK_PLL_DDR1,
    HAL_CLK_PLL_32K,
    HAL_CLK_PLL_PERI0,
    HAL_CLK_PLL_PERI1,
    HAL_CLK_PLL_GPU0,
    HAL_CLK_PLL_GPU1,
    HAL_CLK_PLL_VIDEO0,
    HAL_CLK_PLL_VIDEO1,
    HAL_CLK_PLL_VIDEO2,
    HAL_CLK_PLL_VE,
    HAL_CLK_PLL_DE,
    HAL_CLK_PLL_ISP,
    HAL_CLK_PLL_HSIC,
    HAL_CLK_PLL_AUDIO,
    HAL_CLK_PLL_VIDEO,
    HAL_CLK_PLL_MIPI,
    HAL_CLK_PLL_HDMI,
    HAL_CLK_PLL_USB,
    HAL_CLK_PLL_EDP,
    HAL_CLK_PLL_SATA,
    HAL_CLK_PLL_ADC,
    HAL_CLK_PLL_DTMB,
    HAL_CLK_PLL_24M,
    HAL_CLK_PLL_EVE,
    HAL_CLK_PLL_CVE,
    HAL_CLK_PLL_ISE,
    HAL_CLK_PLL_CSI,

    /*
        PERIPH MODULE CLOCK 1024~2047
    */
    HAL_CLK_BUS_C0_CPU  = HAL_CLK_PERIPH_OFFSET,
    HAL_CLK_BUS_C1_CPU,
    HAL_CLK_BUS_C0_AXI,
    HAL_CLK_BUS_C1_AXI,
    HAL_CLK_BUS_CPUAPB,
    HAL_CLK_BUS_PSI,
    HAL_CLK_BUS_AHB1,
    HAL_CLK_BUS_AHB2,
    HAL_CLK_BUS_AHB3,
    HAL_CLK_BUS_APB1,
    HAL_CLK_BUS_APB2,
    HAL_CLK_BUS_CCI400,
    HAL_CLK_BUS_MBUS,
    HAL_CLK_PERIPH_DMA,
    HAL_CLK_PERIPH_DE,
    HAL_CLK_PERIPH_EE,
    HAL_CLK_PERIPH_DI,
    HAL_CLK_PERIPH_G2D,
    HAL_CLK_PERIPH_EDMA,
    HAL_CLK_PERIPH_EVE,
    HAL_CLK_PERIPH_CVE,
    HAL_CLK_PERIPH_GPU,
    HAL_CLK_PERIPH_CE,
    HAL_CLK_PERIPH_VE,
    HAL_CLK_PERIPH_EISE,
    HAL_CLK_PERIPH_NNA,
    HAL_CLK_PERIPH_NNA_RST,
    HAL_CLK_PERIPH_MSGBOX,
    HAL_CLK_PERIPH_SPINLOCK,
    HAL_CLK_PERIPH_HSTIMER,
    HAL_CLK_PERIPH_AVS,
    HAL_CLK_PERIPH_DBGSYS,
    HAL_CLK_PERIPH_PWM,
    HAL_CLK_PERIPH_IOMMU,
    HAL_CLK_PERIPH_GPIO,
    HAL_CLK_PERIPH_DRAM,
    HAL_CLK_PERIPH_NAND0,
    HAL_CLK_PERIPH_NAND1,
    HAL_CLK_PERIPH_SDMMC0_MOD,
    HAL_CLK_PERIPH_SDMMC0_RST,
    HAL_CLK_PERIPH_SDMMC0_BUS,
    HAL_CLK_PERIPH_SDMMC1_MOD,
    HAL_CLK_PERIPH_SDMMC1_RST,
    HAL_CLK_PERIPH_SDMMC2_BUS,
    HAL_CLK_PERIPH_SDMMC2_MOD,
    HAL_CLK_PERIPH_SDMMC2_RST,
    HAL_CLK_PERIPH_SDMMC1_BUS,
    HAL_CLK_PERIPH_SMHC3,
    HAL_CLK_PERIPH_SMHC4,
    HAL_CLK_PERIPH_SMHC5,
    HAL_CLK_PERIPH_UART0,
    HAL_CLK_PERIPH_UART1,
    HAL_CLK_PERIPH_UART2,
    HAL_CLK_PERIPH_UART3,
    HAL_CLK_PERIPH_UART4,
    HAL_CLK_PERIPH_UART5,
    HAL_CLK_PERIPH_UART6,
    HAL_CLK_PERIPH_UART7,
    HAL_CLK_PERIPH_TWI0,
    HAL_CLK_PERIPH_TWI1,
    HAL_CLK_PERIPH_TWI2,
    HAL_CLK_PERIPH_TWI3,
    HAL_CLK_PERIPH_TWI4,
    HAL_CLK_PERIPH_CAN0,
    HAL_CLK_PERIPH_CAN1,
    HAL_CLK_PERIPH_CAN2,
    HAL_CLK_PERIPH_SCR0,
    HAL_CLK_PERIPH_SCR1,
    HAL_CLK_PERIPH_SCR2,
    HAL_CLK_PERIPH_SCR3,
    HAL_CLK_PERIPH_SPI0,
    HAL_CLK_PERIPH_SPI1,
    HAL_CLK_PERIPH_SPI2,
    HAL_CLK_PERIPH_SPI3,
    HAL_CLK_PERIPH_SPI4,
    HAL_CLK_PERIPH_SPI5,
    HAL_CLK_PERIPH_SPI6,
    HAL_CLK_PERIPH_SPI7,
    HAL_CLK_PERIPH_THS,
    HAL_CLK_PERIPH_GMAC,
    HAL_CLK_PERIPH_EPHY,
    HAL_CLK_PERIPH_EMAC,
    HAL_CLK_PERIPH_SATA,
    HAL_CLK_PERIPH_TS0,
    HAL_CLK_PERIPH_TS1,
    HAL_CLK_PERIPH_IRTX,
    HAL_CLK_PERIPH_KEYPAD,
    HAL_CLK_PERIPH_GPADC,
    HAL_CLK_PERIPH_LEDC,
    HAL_CLK_PERIPH_PIO,
    HAL_CLK_PERIPH_MAD,
    HAL_CLK_PERIPH_LPSD,
    HAL_CLK_PERIPH_DTMB,
    HAL_CLK_PERIPH_I2S0,
    HAL_CLK_PERIPH_I2S1,
    HAL_CLK_PERIPH_I2S2,
    HAL_CLK_PERIPH_SPDIF,
    HAL_CLK_PERIPH_DSD,
    HAL_CLK_PERIPH_DMIC,
    HAL_CLK_PERIPH_AUDIOCODEC_1X,
    HAL_CLK_PERIPH_AUDIOCODEC_4X,
    HAL_CLK_PERIPH_WLAN,
    HAL_CLK_PERIPH_USB0,
    HAL_CLK_PERIPH_USB1,
    HAL_CLK_PERIPH_USB2,
    HAL_CLK_PERIPH_USB3,
    HAL_CLK_PERIPH_USBOHCI0,
    HAL_CLK_PERIPH_USBOHCI0_12M,
    HAL_CLK_PERIPH_USBOHCI1,
    HAL_CLK_PERIPH_USBOHCI1_12M,
    HAL_CLK_PERIPH_USBEHCI0,
    HAL_CLK_PERIPH_USBEHCI1,
    HAL_CLK_PERIPH_USBOTG,
    HAL_CLK_PERIPH_HDMI0,
    HAL_CLK_PERIPH_HDMI1,
    HAL_CLK_PERIPH_HDMI2,
    HAL_CLK_PERIPH_HDMI3,
    HAL_CLK_PERIPH_MIPI_DSI0,
    HAL_CLK_PERIPH_MIPI_DPHY0,
    HAL_CLK_PERIPH_MIPI_HOST0,
    HAL_CLK_PERIPH_MIPI_DSI1,
    HAL_CLK_PERIPH_MIPI_HOST1,
    HAL_CLK_PERIPH_MIPI_DSI2,
    HAL_CLK_PERIPH_MIPI_HOST2,
    HAL_CLK_PERIPH_MIPI_DSI3,
    HAL_CLK_PERIPH_MIPI_HOST3,
    HAL_CLK_PERIPH_MIPI_DSC,
    HAL_CLK_PERIPH_DISPLAY_TOP,
    HAL_CLK_PERIPH_TCON_LCD0,
    HAL_CLK_PERIPH_TCON_LCD1,
    HAL_CLK_PERIPH_TCON_LCD2,
    HAL_CLK_PERIPH_TCON_LCD3,
    HAL_CLK_PERIPH_TCON_TV0,
    HAL_CLK_PERIPH_TCON_TV1,
    HAL_CLK_PERIPH_TCON_TV2,
    HAL_CLK_PERIPH_TCON_TV3,
    HAL_CLK_PERIPH_TVE0,
    HAL_CLK_PERIPH_TVE1,
    HAL_CLK_PERIPH_LVDS,
    HAL_CLK_PERIPH_TVD0,
    HAL_CLK_PERIPH_TVD1,
    HAL_CLK_PERIPH_TVD2,
    HAL_CLK_PERIPH_TVD3,
    HAL_CLK_PERIPH_TVD4,
    HAL_CLK_PERIPH_TVD5,
    HAL_CLK_PERIPH_EDP,
    HAL_CLK_PERIPH_CSI0,
    HAL_CLK_PERIPH_CSI1,
    HAL_CLK_PERIPH_MIPI_CSI,
    HAL_CLK_PERIPH_SUB_LVDS,
    HAL_CLK_PERIPH_HISP,
    HAL_CLK_PERIPH_CSI_TOP,
    HAL_CLK_PERIPH_CSI_MASTER0,
    HAL_CLK_PERIPH_CSI_MASTER1,
    HAL_CLK_PERIPH_ISP,
    HAL_CLK_PERIPH_DSPO,
} hal_clk_id_t, *hal_clk_id_pt;

typedef hal_clk_id_t hal_clk_t;
#define HAL_SUNXI_CCU (0)

// typedef hal_clk_id_t CSP_CCM_sysClkNo_t;
// typedef hal_clk_id_t CSP_CCM_modClkNo_t;

/************************************************************************************************
* @Function: hal_clock_init
* @Description: provide HAL API for initialize soc clocks during the system power-on startup phase
* @Parameters:
* # void: No parameters required
* @Return values:
* # HAL_CLK_STATUS_OK: soc clocks initialize successed
* # others : soc clocks initialization may have some abnormal problems
* @Attention: clock initialize timing depands on specific soc platform clock design
*************************************************************************************************/
void hal_clock_init(void);

hal_clk_t hal_clock_get(hal_clk_type_t type, hal_clk_id_t id);


/************************************************************************************************
* @Function: hal_clock_is_enabled
* @Description: provide HAL API for bus-clk and periph-clk to get clock enabled statue
* @Parameters:
* # clk: clock-id of soc specific clock
* @Return values:
* # HAL_CLK_STATUS_INVALID_PARAMETER: input parameter of clock-id undefined in hal ot rate value is invalid
* # HAL_CLK_STATUS_ERROR_CLK_SET_RATE_REFUSED: fixed-clk and factor clk not allowed User to change rate because of stability
* # HAL_CLK_STATUS_ERROT_CLK_UNDEFINED: input parameter of clock-id defined in hal but not defined by soc clock driver
* # HAL_CLK_STATUS_ERROR_CLK_NOT_FOUND: input parameter of clock-id defined in hal but not defined by soc clock driver
* # HAL_CLK_STATUS_ENABLED: clock current status is enabled
* # HAL_CLK_STATUS_DISABLED: clock current status is disabled
* @Attention: .etc
*************************************************************************************************/
hal_clk_status_t hal_clock_is_enabled(hal_clk_id_t clk);

/************************************************************************************************
* @Function: hal_clock_enable
* @Description: provide HAL API for bus-clk and periph-clk to enable clock
* @Parameters:
* # clk: clock-id of soc specific clock
* @Return values:
* # HAL_CLK_STATUS_INVALID_PARAMETER: input parameter of clock-id undefined in hal ot rate value is invalid
* # HAL_CLK_STATUS_ERROR_CLK_SET_RATE_REFUSED: fixed-clk and factor clk not allowed User to change rate because of stability
* # HAL_CLK_STATUS_ERROT_CLK_UNDEFINED: input parameter of clock-id defined in hal but not defined by soc clock driver
* # HAL_CLK_STATUS_ERROR_CLK_NOT_FOUND: input parameter of clock-id defined in hal but not defined by soc clock driver
* # HAL_CLK_STATUS_ENABLED: clock current status is enabled
* # HAL_CLK_STATUS_DISABLED: clock current status is disabled
* @Attention: .etc
*************************************************************************************************/
hal_clk_status_t hal_clock_enable(hal_clk_id_t clk);


/************************************************************************************************
* @Function: hal_clock_disable
* @Description: provide HAL API for bus-clk and periph-clk to disable clock
* @Parameters:
* # clk: clock-id of soc specific clock
* @Return values:
* # HAL_CLK_STATUS_INVALID_PARAMETER: input parameter of clock-id undefined in hal ot rate value is invalid
* # HAL_CLK_STATUS_ERROR_CLK_SET_RATE_REFUSED: fixed-clk and factor clk not allowed User to change rate because of stability
* # HAL_CLK_STATUS_ERROT_CLK_UNDEFINED: input parameter of clock-id defined in hal but not defined by soc clock driver
* # HAL_CLK_STATUS_ERROR_CLK_NOT_FOUND: input parameter of clock-id defined in hal but not defined by soc clock driver
* # HAL_CLK_STATUS_OK: clock current status disabled successed
* @Attention: .etc
*************************************************************************************************/
hal_clk_status_t hal_clock_disable(hal_clk_id_t clk);


/************************************************************************************************
* @Function: hal_clk_recalc_rate
* @Description: provide HAL API for factor-clk, bus-clk and periph-clk to recalculate current Runtime rate
* @Parameters:
* # clk: clock-id of soc specific clock
* @Return values:
* # HAL_CLK_STATUS_INVALID_PARAMETER: input parameter of clock-id undefined in hal
* # HAL_CLK_RATE_UNINITIALIZED : input parameter of clock-id defined in hal but not defined by soc clock driver or clock disbaled
* # others: return current clock rate successed
* @Attention: .etc
*************************************************************************************************/
u32 hal_clk_recalc_rate(hal_clk_id_t clk);


/************************************************************************************************
* @Function: hal_clk_round_rate
* @Description: provide HAL API for factor-clk, bus-clk and periph-clk round target rate to the most suitable rate
* @Parameters:
* # clk: clock-id of soc specific clock
* # rate: the target rate form API-User
* @Return values:
* # HAL_CLK_STATUS_INVALID_PARAMETER: input parameter of clock-id undefined in hal ot rate value is invalid
* # HAL_CLK_RATE_UNINITIALIZED : input parameter of clock-id defined in hal but not defined by soc clock driver or clock disbaled
* # others: return round rate successed
* @Attention: .etc
*************************************************************************************************/
u32 hal_clk_round_rate(hal_clk_id_t clk, u32 rate);


/************************************************************************************************
* @Function: hal_clk_get_rate
* @Description: provide HAL API for factor-clk, bus-clk and periph-clk get current rate cached witch may not current Runtime rate
* @Parameters:
* # clk: clock-id of soc specific clock
* @Return values:
* # HAL_CLK_STATUS_INVALID_PARAMETER: input parameter of clock-id undefined in hal ot rate value is invalid
* # HAL_CLK_RATE_UNINITIALIZED : input parameter of clock-id defined in hal but not defined by soc clock driver or clock disbaled
* # others: return rate cached successed
* @Attention: .etc
*************************************************************************************************/
u32  hal_clk_get_rate(hal_clk_id_t clk);


/************************************************************************************************
* @Function: hal_clk_set_rate
* @Description: provide HAL API for bus-clk and periph-clk to set new rate
* @Parameters:
* # clk: clock-id of soc specific clock
* # rate: the new rate value
* @Return values:
* # HAL_CLK_STATUS_INVALID_PARAMETER: input parameter of clock-id undefined in hal ot rate value is invalid
* # HAL_CLK_STATUS_ERROR_CLK_SET_RATE_REFUSED: fixed-clk and factor clk not allowed User to change rate because of stability
* # HAL_CLK_STATUS_ERROT_CLK_UNDEFINED: input parameter of clock-id defined in hal but not defined by soc clock driver
* # HAL_CLK_STATUS_ERROR_CLK_NOT_FOUND: input parameter of clock-id defined in hal but not defined by soc clock driver
* # HAL_CLK_STATUS_OK: set new rate successed
* @Attention: .etc
*************************************************************************************************/
hal_clk_status_t hal_clk_set_rate(hal_clk_id_t clk,  u32 rate);


/************************************************************************************************
* @Function: hal_clk_set_parent
* @Description: provide HAL API for factor-clk, bus-clk and periph-clk to select parent clock
* @Parameters:
* # clk: clock-id of soc specific clock witch nedds to adjust parent clock
* # parent: clock-id of soc specific clock's parent clock
* @Return values:
* # HAL_CLK_STATUS_OK: soc specific clock select and siwtch parent clock successed
* # others : soc specific clock select and siwtch parent clock may have some abnormal problems
* @Attention: soc specific clock and parent clock must be according to the SOC_User_Manual definition
*************************************************************************************************/
hal_clk_status_t hal_clk_set_parent(hal_clk_id_t clk, hal_clk_id_t parent);


/************************************************************************************************
* @Function: hal_clk_get_parent
* @Description: provide HAL API for factor-clk, bus-clk and periph-clk to get current parent clock
* @Parameters:
* # clk: clock-id of soc specific clock
* @Return values:
* # HAL_CLK_STATUS_INVALID_PARAMETER: input parameter of clock-id undefined in hal
* # HAL_CLK_UNINITIALIZED : input parameter of clock-id defined in hal but not defined by soc clock driver
* # others: return current parent clock-id successed
* @Attention: soc specific clock and parent clock must be according to the SOC_User_Manual definition
*************************************************************************************************/
hal_clk_id_t hal_clk_get_parent(hal_clk_id_t clk);


#endif /* __HAL_CLOCK_H__ */

