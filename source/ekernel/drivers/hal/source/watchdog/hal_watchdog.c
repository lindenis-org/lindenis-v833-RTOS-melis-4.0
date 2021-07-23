/*
 * Copyright 2018 Rockchip Electronics Co., Ltd
 *
 */
#include <sunxi_hal_watchdog.h>
#include <stdio.h>

static const int WTD_timeout_map[] =
{
    [1]  = 0x1,  /* 1s  32000cycles*/
    [2]  = 0x2,  /* 2s  64000*/
    [3]  = 0x3,  /* 3s  96000*/
    [4]  = 0x4,  /* 4s  128000*/
    [5]  = 0x5,  /* 5s  160000*/
    [6]  = 0x6,  /* 6s  192000*/
    [8]  = 0x7,  /* 8s  256000*/
    [10] = 0x8,  /* 10s 320000*/
    [12] = 0x9,  /* 12s 384000*/
    [14] = 0xA,  /* 14s 448000*/
    [16] = 0xB,  /* 16s 512000*/
};


void hal_watchdog_info(void)
{
    struct hal_sunxi_wtd *wtd= (struct hal_sunxi_wtd *)WTD_BASE;

	printf("mode: 0x%lx,cfg:0x%lx,ctl:0x%lx \n",wtd->mode,wtd->cfg,wtd->ctl);
	
}

void hal_watchdog_disable(void)
{
    struct hal_sunxi_wtd *WTD = (struct hal_sunxi_wtd *)WTD_BASE;
    unsigned int wtmode;

    wtmode = readl(&WTD->mode);
    wtmode &= ~WTD_MODE_EN;

    writel(wtmode, &WTD->mode);
}

void hal_watchdog_reset(int timeout)
{
    int timeout_set = timeout;
    struct hal_sunxi_wtd *WTD = (struct hal_sunxi_wtd *)WTD_BASE;
    unsigned int wtmode;

    hal_watchdog_disable();

    if (timeout > 16)
    {
        timeout_set = 16;
    }

    if (WTD_timeout_map[timeout_set] == 0)
    {
        timeout_set++;
    }

    wtmode = WTD_timeout_map[timeout] << 4 | WTD_MODE_EN;

    writel(WTD_CFG_RESET, &WTD->cfg);
    writel(wtmode, &WTD->mode);
    writel(WTD_CTRL_KEY | WTD_CTRL_RESTART, &WTD->ctl);
}

void hal_watchdog_restart(void)
{
    struct hal_sunxi_wtd *WTD = (struct hal_sunxi_wtd *)WTD_BASE;

    /* Set the watchdog for its shortest interval (.5s) and wait */
    writel(WTD_CFG_RESET, &WTD->cfg);
    writel(WTD_MODE_EN, &WTD->mode);

    while (1) { };
}




