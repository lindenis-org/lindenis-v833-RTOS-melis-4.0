/*
 * ===========================================================================================
 *
 *       Filename:  spi_dev.c
 *
 *    Description:  implemtaton of spi driver core based on hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-25 17:31:59
 *       Revision:  none
 *       Compiler:  GCC:version 9.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  bantao@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2019-12-03 15:57:57
 *
 * ===========================================================================================
 */
#include <sunxi_hal_spi.h>
#include <sunxi_drv_spi.h>
#include <sunxi_drv_list.h>
#include <rtthread.h>
#include <log.h>
#include <init.h>


// bring hal layer in.
extern const sunxi_hal_driver_spi_t sunxi_hal_spi_driver;
static sunxi_driver_spi_t spi0, spi1, spi2, spi3;

static rt_err_t spi_init(struct rt_device *dev)
{
    return 0;
}

static rt_err_t spi_open(struct rt_device *dev, rt_uint16_t oflag)
{
    rt_size_t ret = 0;
    sunxi_driver_spi_t *pusr, *pvfy;
    sunxi_hal_driver_spi_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_spi_t, base);
    pusr = (sunxi_driver_spi_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_spi_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->initialize)
    {
        ret = hal_drv->initialize(pusr->dev_id, NULL);
    }

    return ret;
}

static rt_size_t spi_write(struct rt_device *dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    rt_size_t ret = 0;
    sunxi_driver_spi_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_spi_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_spi_t, base);
    pusr = (sunxi_driver_spi_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_spi_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->send)
    {
        ret = hal_drv->send(pusr->dev_id, buffer, size);
    }

    return ret;
    return 0;
}

static rt_err_t spi_close(struct rt_device *dev)
{
    rt_size_t ret = 0;
    sunxi_driver_spi_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_spi_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_spi_t, base);
    pusr = (sunxi_driver_spi_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_spi_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->uninitialize)
    {
        ret = hal_drv->uninitialize(pusr->dev_id);
    }

    return ret;
}

static rt_size_t spi_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_size_t ret = 0;
    sunxi_driver_spi_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_spi_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_spi_t, base);
    pusr = (sunxi_driver_spi_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_spi_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->receive)
    {
        ret = hal_drv->receive(pusr->dev_id, buffer, size);
    }

    return ret;
}

static rt_err_t spi_control(struct rt_device *dev, int cmd, void *args)
{
    rt_size_t ret = 0;
    sunxi_driver_spi_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_spi_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = container_of(dev, sunxi_driver_spi_t, base);
    pusr = (sunxi_driver_spi_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_spi_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->control)
    {
        ret = hal_drv->control(pusr->dev_id, cmd, args);
    }

    return ret;
}

static void init_spi_device(struct rt_device *dev, void *usr_data, char *dev_name)
{
    rt_err_t ret;

    dev->open       = spi_open;
    dev->init       = spi_init;
    dev->write      = spi_write;
    dev->close      = spi_close;
    dev->read       = spi_read;
    dev->control    = spi_control;
    dev->user_data  = usr_data;

    ret = rt_device_register(dev, dev_name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        __err("fatal error, ret %d, register device to rtsystem failure.\n", ret);
        while (1);
        return;
    }
}

int  sunxi_driver_spi_init(void)
{
    struct rt_device *device0, *device1, *device2, *device3;

    device0 = &spi0.base;
    spi0.dev_id = 0;
    spi0.hal_drv = &sunxi_hal_spi_driver;

    device1 = &spi1.base;
    spi1.dev_id = 1;
    spi1.hal_drv = &sunxi_hal_spi_driver;

    device2 = &spi2.base;
    spi2.dev_id = 2;
    spi2.hal_drv = &sunxi_hal_spi_driver;

    device3 = &spi3.base;
    spi3.dev_id = 3;
    spi3.hal_drv = &sunxi_hal_spi_driver;

    init_spi_device(device0, &spi0, "spi0");
    init_spi_device(device1, &spi1, "spi1");
    init_spi_device(device2, &spi2, "spi2");
    init_spi_device(device3, &spi3, "spi3");

    return 0;
}

late_initcall(sunxi_driver_spi_init);

