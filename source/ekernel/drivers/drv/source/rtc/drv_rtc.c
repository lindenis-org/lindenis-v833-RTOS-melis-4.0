/*
 * Copyright 2018 Rockchip Electronics Co., Ltd
 *
 */
#include <sunxi_hal_rtc.h>
#include <sunxi_drv_rtc.h>

#include <rtthread.h>
#include <log.h>
#include <init.h>

extern const sunxi_hal_driver_rtc_t sunxi_hal_rtc_driver;
static sunxi_driver_rtc_t rtc;

static rt_err_t rtc_open(struct rt_device *dev, rt_uint16_t oflag)
{
    rt_size_t ret = 0;
    sunxi_driver_rtc_t *pusr, *pvfy;
    sunxi_hal_driver_rtc_t *hal_drv = NULL;

    if (dev == NULL)
    {
        return -1;
    }

    pvfy = rt_container_of(dev, sunxi_driver_rtc_t, base);
    pusr = (sunxi_driver_rtc_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_rtc_t *)pusr->hal_drv;
    }

    return ret;
}

static rt_err_t rtc_close(struct rt_device *dev)
{
    rt_size_t ret = 0;
    sunxi_driver_rtc_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_rtc_t *hal_drv = NULL;

    if (dev == NULL)
    {
        return -1;
    }

    pvfy = rt_container_of(dev, sunxi_driver_rtc_t, base);
    pusr = (sunxi_driver_rtc_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_rtc_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->uninitialize)
    {
        ret = hal_drv->uninitialize();
    }

    return ret;
}

static rt_err_t rtc_init(struct rt_device *dev)
{
    rt_size_t ret = 0;
    sunxi_driver_rtc_t *pusr, *pvfy;
    sunxi_hal_driver_rtc_t *hal_drv = NULL;

    if (dev == NULL)
    {
        return -1;
    }

    pvfy = rt_container_of(dev, sunxi_driver_rtc_t, base);
    pusr = (sunxi_driver_rtc_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_rtc_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->initialize)
    {
        ret = hal_drv->initialize();
    }

    return 0;
}

static rt_size_t rtc_write(struct rt_device *dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
	return 0;
}

static rt_size_t rtc_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
	return 0;
}

static rt_err_t rtc_control(struct rt_device *dev, int cmd, void *args)
{
    rt_size_t ret = 0;
    sunxi_driver_rtc_t *pusr = NULL, *pvfy = NULL;
    sunxi_hal_driver_rtc_t *hal_drv = NULL;

    if (dev == NULL)
    {
        while (1);
    }

    pvfy = rt_container_of(dev, sunxi_driver_rtc_t, base);
    pusr = (sunxi_driver_rtc_t *)dev->user_data;

    if (pusr)
    {
        hal_drv = (sunxi_hal_driver_rtc_t *)pusr->hal_drv;
    }

    if (hal_drv && hal_drv->control)
    {
        ret = hal_drv->control(cmd, args);
    }

    return ret;
}

#if defined(RT_USING_POSIX)
#include <dfs_file.h>
#include <rtdevice.h>

static int sunxi_rtc_fops_open(struct dfs_fd *fd)
{
    rt_device_t dev;

    dev = (rt_device_t) fd->data;
    dev->open(dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);

    fd->size = 0;
    fd->type  = FT_DEVICE;

    return rtc_open(dev, 0);
}

static int sunxi_rtc_fops_close(struct dfs_fd *fd)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return rtc_close(dev);
}

static int sunxi_rtc_fops_ioctl(struct dfs_fd *fd, int cmd, void *args)
{
    rt_device_t dev;
    dev = (rt_device_t) fd->data;

    return rtc_control(dev, cmd, args);
}

static int sunxi_rtc_fops_read(struct dfs_fd *fd, void *buf, size_t count)
{
    rt_device_t dev;
    int ret = -1;
    dev = (rt_device_t) fd->data;

    ret = dev->read(dev, fd->pos, buf, count);
    if (ret >= 0)
    {
        fd->pos += ret;
    }

    return ret;
}

static int sunxi_rtc_fops_write(struct dfs_fd *fd, const void *buf, size_t count)
{
    rt_device_t dev;
    int ret = -1;
    dev = (rt_device_t) fd->data;

    ret = dev->write(dev, fd->pos, buf, count);
    if (ret >= 0)
    {
        fd->pos += ret;
    }

    return ret;
}

static struct dfs_file_ops rtc_device_fops =
{
    .open = sunxi_rtc_fops_open,
    .close = sunxi_rtc_fops_close,
    .ioctl = sunxi_rtc_fops_ioctl,
    .read = sunxi_rtc_fops_read,
    .write = sunxi_rtc_fops_write,
};
#endif

static rt_err_t init_rtc_device(struct rt_device *dev, void *usr_data, char *dev_name)
{
    rt_err_t ret;

    dev->open       = rtc_open;
    dev->close      = rtc_close;
    dev->init       = rtc_init;
    dev->write      = rtc_write;
    dev->read       = rtc_read;
    dev->control    = rtc_control;
    dev->user_data  = usr_data;

    ret = rt_device_register(dev, dev_name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        __err("fatal error, ret %d, register device to rtsystem failure.\n", ret);
        return ret;
    }

    ret = rt_device_init(dev);

#if defined(RT_USING_POSIX)
    /* dev->fops = &rtc_device_fops; */
#endif
    return ret;
}

int sunxi_driver_rtc_init(void)
{
    struct rt_device *device;

    device = &rtc.base;
    rtc.dev_id = 0;
    rtc.hal_drv = &sunxi_hal_rtc_driver;

    init_rtc_device(device, &rtc, "rtc");

    return 0;
}

subsys_initcall(sunxi_driver_rtc_init);
