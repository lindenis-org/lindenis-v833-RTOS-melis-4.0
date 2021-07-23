/*
 * ===========================================================================================
 *
 *       Filename:  sunxi_drv_uart.h
 *
 *    Description:  sunxi_driver headfile.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-14 17:47:21
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-15 13:19:52
 *
 * ===========================================================================================
 */
#ifndef SUNXI_DRV_UART_H
#define SUNXI_DRV_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>
#include <sunxi_hal_usart.h>

typedef struct sunxi_driver_uart
{
    struct rt_device   base;
    int32_t            dev_id;
    const void        *hal_drv;
} sunxi_driver_uart_t;

void sunxi_driver_uart_init(void);

#ifdef __cplusplus
}
#endif

#endif
