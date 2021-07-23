
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

#ifndef __SUNXI_HAL_PWM_H__
#define __SUNXI_HAL_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <typedef.h>
#include <init.h>
#include "sun8i/pwm-sun8iw19.h"

#define CONFIG_DRIVERS_PWM_DEBUG
#ifdef CONFIG_DRIVERS_PWM_DEBUG
#define PWM_INFO(fmt, arg...) __log(fmt, ##arg)
#else
#define PWM_INFO(fmt, arg...) do {}while(0)
#endif

#define PWM_ERR(fmt, arg...) __err(fmt, ##arg)

#define readl(addr)     (*((volatile unsigned long  *)(addr)))
#define writel(v, addr) (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

/*****************************************************************************
 * Enums
 *****************************************************************************/

typedef unsigned long pwm_status_t;

typedef enum
{
    PWM_CLK_OSC,
    PWM_CLK_APB,
} hal_pwm_clk_src;

typedef enum
{
    PWM_POLARITY_INVERSED = 0,
    PWM_POLARITY_NORMAL = 1,
} hal_pwm_polarity;

typedef enum
{
    PWM_CONTROL = 0,
    PWM_CHANNEL_INT = 1,
    PWM_CHANNEL_UNINT = 2,
} hal_pwm_cmd_t;

typedef struct pwm_config
{
    uint32_t        duty_ns;
    uint32_t        period_ns;
    hal_pwm_polarity    polarity;
} pwm_config_t;

typedef struct sunxi_hal_pwm_driver
{
    pwm_status_t (*initialize)(void);
    pwm_status_t (*control)(int channel, struct pwm_config *config_pwm);
    pwm_status_t (*uninitialize)(void);

} sunxi_hal_driver_pwm_t;


pwm_status_t hal_pwm_init(void);
pwm_status_t hal_pwm_control(int channel, struct pwm_config *config_pwm);
void hal_pwm_enable_controller(uint32_t channel_in);
void hal_pwm_disable_controller(uint32_t channel_in);
pwm_status_t hal_pwm_uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* __SUNXI_HAL_PWM_H__ */


