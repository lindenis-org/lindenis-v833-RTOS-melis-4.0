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

#include <sunxi_drv_timer.h>
#include <interrupt.h>

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
drv_timer_status_t drv_timer_set_pres(u32 timer_id, u32 pres_mode)
{
    if (hal_check_valid_timer_pres(timer_id) < 0)
    {
        return DRV_TMR_ID_ERROR;
    }

    if (hal_check_valid_timer_pres(pres_mode) < 0)
    {
        return DRV_TMR_PRES_ERROR;
    }

    if (hal_timer_set_pres(timer_id, pres_mode) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}


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
drv_timer_status_t drv_timer_set_mode(u32 timer_id, u32 timer_mode)
{
    if (hal_check_valid_timer_id(timer_id) < 0)
    {
        return DRV_TMR_ID_ERROR;
    }

    if (hal_check_valid_timer_mode(timer_mode) < 0)
    {
        return DRV_TMR_MODE_ERROR;
    }

    if (hal_timer_set_mode(timer_id, timer_mode) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}

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
drv_timer_status_t drv_timer_get_counter(u32 timer_id, u32 *value)
{
    if (hal_check_valid_timer_id(timer_id) < 0)
    {
        return DRV_TMR_ID_ERROR;
    }

    if (hal_timer_get_counter(timer_id, value) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}

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
drv_timer_status_t drv_timer_set_counter(u32 timer_id, u32 value)
{
    if (hal_check_valid_timer_id(timer_id) < 0)
    {
        return DRV_TMR_ID_ERROR;
    }

    if (hal_timer_set_counter(timer_id, value) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}

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
drv_timer_status_t drv_timer_set_interval(u32 timer_id, u32 value)
{
    if (hal_check_valid_timer_id(timer_id) < 0)
    {
        return DRV_TMR_ID_ERROR;
    }

    if (hal_timer_set_interval(timer_id, value) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}

/**
 * @dui       This function put the timer_id to the irq;
 * @param[in] timer_id specifies the timer id.
 * @return    the irq of the timer
 * @note
 * @warning
 */
u32 drv_timer_id_to_irq(u32 timer_id)
{
    if (hal_check_valid_timer_id(timer_id) < 0)
    {
        return DRV_TMR_ID_ERROR;
    }

    return hal_timer_id_to_irq(timer_id);
}


/**
 * @dui       This function request an irq of timer.
 * @param[in] irq specifies the irq to request.
 * @param[in] hdle specifies the irq handler
 * @param[in] flags specifiles the irq detach type
 * @param[in] data specifiles the irq handler data
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_irq_request(u32 irq, irq_handler_t hdle, unsigned long flags, void *data)
{
    if (hal_timer_irq_request(irq, hdle, flags, data) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}

/**
 * @dui       This function free an irq of timer.
 * @param[in] irq specifies the irq to free.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_irq_free(u32 irq)
{

    if (hal_timer_irq_free(irq) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}

/**
 * @dui       This function enable an irq of the timer.
 * @param[in] irq specifies the irq to enable.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_irq_enable(u32 irq)
{
    if (hal_timer_irq_enable(irq) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}


/**
 * @dui       This function disable an irq of the timer.
 * @param[in] irq specifies the irq to enable.
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_irq_disable(u32 irq)
{
    if (hal_timer_irq_disable(irq) < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}

/**
 * @dui       This function init the timer.
 * @param[in] none
 * @return    To indicate whether this function call is successful or not.
 *            If the return value is #DRV_TMR_STATUS_OK, the operation completed successfully.
 *            If the return value is #DRV_TMR_STATUS_ERROR, the operation failed.
 * @note
 * @warning
 */
drv_timer_status_t drv_timer_init(void)
{
    if (hal_timer_init() < 0)
    {
        return DRV_TMR_STATUS_ERROR;
    }

    return DRV_TMR_STATUS_OK;
}

