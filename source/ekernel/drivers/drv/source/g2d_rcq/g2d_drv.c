/*
 * g2d_drv.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 *
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <hal_clk.h>
#include <interrupt.h>
#include <init.h>
#include <stdlib.h>
#include <string.h>
#include <sunxi_hal_common.h>
#include <hal_sem.h>
#include "../../../hal/source/g2d_rcq/g2d_driver_i.h"
#include "../../../hal/source/g2d_rcq/g2d_top.h"
#include "../../../hal/source/g2d_rcq/g2d_rotate.h"
#include "../../../hal/source/g2d_rcq/g2d_mixer.h"
#define SUNXI_GIC_START 32
#define SUNXI_IRQ_G2D (SUNXI_GIC_START + 21)
#define SUNXI_G2D_START 0x01480000


extern __g2d_info_t para;
extern __u32 dbg_info;
extern __g2d_drv_t g2d_ext_hd;
extern enum g2d_scan_order scan_order;
extern hal_sem_t global_lock;
extern int g2d_mutex_lock(hal_sem_t sem);
extern int g2d_mutex_unlock(hal_sem_t sem);
extern rt_err_t sunxi_g2d_control(rt_device_t dev, int cmd, void *arg);
extern irqreturn_t g2d_handle_irq(int irq, void *dev_id);
__s32 drv_g2d_init(void)
{
	int ret;
	memset(&g2d_ext_hd, 0, sizeof(__g2d_drv_t));

	ret = hal_sem_create(&g2d_ext_hd.queue_sem, 0);
	if (ret < 0)
	{
		G2D_ERR_MSG("create g2d_ext_hd.queue_sem failed\n");
		return ret;
	}


	g2d_top_set_base((__u32)para.io);
	g2d_rot_set_base((__u32)para.io);
	g2d_mixer_idr_init();
	return 0;
}
static rt_err_t sunxi_g2d_init(rt_device_t dev)
{
	return 0;
}

int g2d_clk_init(__g2d_info_t *info)
{
	int ret = -1;
	if (!info)
		goto OUT;

	info->clk = HAL_CLK_UNINITIALIZED;
	info->clk_parent = HAL_CLK_UNINITIALIZED;
	info->clk_rate = 0;
#ifdef CONFIG_SOC_SUN8IW19
	info->clk = HAL_CLK_PERIPH_G2D;
	info->clk_parent = HAL_CLK_PLL_PERI1;
	info->clk_rate = 300000000; /*300Mhz*/
#endif
	ret = hal_clk_set_parent(info->clk, info->clk_parent); 
	if (ret)
		G2D_ERR_MSG("set clk:%d's parent:%d fail!\n", info->clk, info->clk_parent);
OUT:
	return ret;
}

int g2d_clock_enable(__g2d_info_t *info)
{
	int ret = -1;

	if (!info || (info->clk < 0) || (info->clk_parent < 0)) {
		G2D_ERR_MSG("Invalid param\n");
		goto OUT;
	}

	ret = hal_clock_enable(info->clk_parent);
	if (ret) {
		G2D_ERR_MSG("Enable clk parent fail:%d\n", ret);
		goto OUT;
	}

	ret = hal_clk_set_rate(info->clk, info->clk_rate);

	if (ret) {
		G2D_ERR_MSG("Set clk rate fail:%d:%u!\n", info->clk, (unsigned int)info->clk_rate);
		goto OUT;
	}

	ret = hal_clock_enable(info->clk);
	if (ret) {
		G2D_ERR_MSG("Enable clk %d fail:%d\n", info->clk, ret);
		goto OUT;
	}

OUT:
	return ret;
}

static int g2d_clock_disable(__g2d_info_t *info)
{
	int ret = -1;

	if (!info || info->clk < 0) {
		G2D_ERR_MSG("Invalid param\n");
		goto OUT;
	}

	ret = hal_clock_disable(info->clk);
	if (ret) {
		G2D_ERR_MSG("Disable clk %d fail:%d\n", info->clk, ret);
		goto OUT;
	}

OUT:
	return  ret;
}


static rt_err_t sunxi_g2d_open(rt_device_t dev, rt_uint16_t oflag)

{

	hal_clk_status_t ret;


	g2d_mutex_lock(para.mutex);
	g2d_clock_enable(&para);
	para.user_cnt++;
	if (para.user_cnt == 1) {
		para.opened = true;
		g2d_bsp_open();
	}
	g2d_mutex_unlock(para.mutex);
	return 0;
}


static rt_err_t sunxi_g2d_close(rt_device_t dev)
{
	g2d_mutex_lock(para.mutex);
	para.user_cnt--;
	if (para.user_cnt == 0) {
		para.opened = false;
		g2d_bsp_close();
	}
	g2d_clock_disable(&para);

	g2d_mutex_unlock(para.mutex);

	g2d_mutex_lock(global_lock);
	scan_order = G2D_SM_TDLR;
	g2d_mutex_unlock(global_lock);

	return 0;
}



static int g2d_probe(void)
{
	int ret = 0;
	struct rt_device *device;
	
	__g2d_info_t *info = NULL;

	info = &para;

	device = rt_device_create(RT_Device_Class_Graphic, 0);
	if (!device)
	{
		return ret;
	}
	memset(device, 0, sizeof(struct rt_device));
	memset(info, 0, sizeof(__g2d_info_t));
	

	device->init = sunxi_g2d_init;
	device->open = sunxi_g2d_open;
	device->close = sunxi_g2d_close;

	device->control = sunxi_g2d_control;



	ret = rt_device_register(device, "g2d", RT_DEVICE_FLAG_RDWR);
	if (ret != 0)
	{
		return ret;
	}

	info->io = SUNXI_G2D_START;

	if (request_irq(SUNXI_IRQ_G2D, g2d_handle_irq, 0, "g2d", NULL))
        {
        	G2D_ERR_MSG("g2d request irq error\n");
        	return -1;
        }
	enable_irq(SUNXI_IRQ_G2D);

	ret = g2d_clk_init(info);
	drv_g2d_init();
	ret = hal_sem_create(&info->mutex, 1);
	ret = hal_sem_create(&global_lock, 1);

	if (ret < 0)
		G2D_ERR_MSG("sysfs_create_file fail!\n");

	return 0;

	return ret;
}
static int g2d_remove(void)
{

	g2d_mixer_idr_remove();
	INFO("Driver unloaded succesfully.\n");
	return 0;
}
device_initcall(g2d_probe);
