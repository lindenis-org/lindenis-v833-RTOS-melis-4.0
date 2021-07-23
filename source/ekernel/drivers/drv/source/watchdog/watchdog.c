/*
 * Copyright 2018 Rockchip Electronics Co., Ltd
 *
 */
#include <sunxi_hal_watchdog.h>
#include <sunxi_drv_watchdog.h>

void watchdog_disable(void)
{
    hal_watchdog_disable();
}

void watchdog_reset(int timeout)
{
    hal_watchdog_reset(timeout);
}

void reset_cpu(void)
{

    hal_watchdog_restart();
}
