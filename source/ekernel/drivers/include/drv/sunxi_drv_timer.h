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

#ifndef __DRV_TIMER_H__
#define __DRV_GPIO_H__

#include <interrupt.h>
#include "../hal/sunxi_hal_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TIMER0 0
#define TIMER1 1

typedef enum
{
    DRV_TMR_PRES_ERROR = -4,
    DRV_TMR_MODE_ERROR = -3,
    DRV_TMR_ID_ERROR = -2,
    DRV_TMR_STATUS_ERROR = -1,
    DRV_TMR_STATUS_OK = 0
} drv_timer_status_t;

typedef enum
{
    TMR_CONTINUE_MODE,
    TMR_SINGLE_MODE,
} drv_timer_mode;

typedef enum
{
    TMR_1_PRES,
    TMR_2_PRES,
    TMR_4_PRES,
    TMR_8_PRES,
    TMR_16_PRES,
    TMR_32_PRES,
    TMR_64_PRES,
    TMR_128_PRES,
} hal_timer_pres;


/**
 * @dui       This function set the timer pres_mode.
 * @param[in] timer_id specifies the timer id.
 * @param[in] pres_mode specifies the pres mode.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_set_pres(u32 timer_id, u32 pres_mode);

/**
 * @dui       This function set the timer pres_mode.
 * @param[in] timer_id specifies the timer id.
 * @param[in] pres_mode specifies the pres mode.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_set_mode(u32 timer_id, u32 timer_mode);

/**
 * @dui       This function get the counter.
 * @param[in] timer_id specifies the timer id.
 * @param[in] value specifies the value get from counter
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_get_counter(u32 timer_id, u32 *value);

/**
 * @dui       This function get the counter.
 * @param[in] timer_id specifies the timer id.
 * @param[in] value specifies the value set to counter
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_set_counter(u32 timer_id, u32 value);

/**
 * @dui       This function set the timer interval.
 * @param[in] timer_id specifies the timer id.
 * @param[in] pres_mode specifies value set to interval.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_set_interval(u32 timer_id, u32 value);

/**
 * @dui       This function put the timer_id to the irq;
 * @param[in] timer_id specifies the timer id.
 * @return    the irq of the timer
 * @note
 * @warning
 */
u32 drv_timer_id_to_irq(u32 timer_id);

/**
 * @dui       This function request an irq of timer.
 * @param[in] irq specifies the irq to request.
 * @param[in] hdle specifies the irq handler
 * @param[in] flags specifiles the irq detach type
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_irq_request(u32 irq, irq_handler_t hdle, unsigned long flags, void *data);

/**
 * @dui       This function free an irq of timer.
 * @param[in] irq specifies the irq to free.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_irq_free(u32 irq);

/**
 * @dui       This function enable an irq of the timer.
 * @param[in] irq specifies the irq to enable.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_irq_enable(u32 irq);

/**
 * @dui       This function disable an irq of the timer.
 * @param[in] irq specifies the irq to enable.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_irq_disable(u32 irq);

/**
 * @dui       This function init the timer.
 * @param[in] none
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_init(void);

#ifdef __cplusplus
}
#endif

#endif
