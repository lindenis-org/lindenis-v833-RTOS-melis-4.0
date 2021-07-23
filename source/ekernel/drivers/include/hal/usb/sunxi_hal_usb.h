/*
 * ===========================================================================================
 *
 *       Filename:  sunxi_usb_common.h
 *
 *    Description:  USB HAL definition.
 * ===========================================================================================
 */
#ifndef SUNXI_HAL_USB_H
#define SUNXI_HAL_USB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*usb base address*/
#define SUNXI_USB_OTG_PBASE	0x05100000

/*usb irq*/
#define SUNXI_IRQ_USBOTG		96

#endif
