/*
 * Copyright 2018 Rockchip Electronics Co., Ltd
 *
 */
#include <sunxi_hal_watchdog.h>
#include <sunxi_drv_watchdog.h>

void drv_watchdog_info()
{
    hal_watchdog_info();
}

void drv_watchdog_disable(void)
{
    hal_watchdog_disable();
	
}

void drv_watchdog_reset(int timeout)
{
    hal_watchdog_reset(timeout);

}

void reset_cpu(void)
{

    hal_watchdog_restart();

}
