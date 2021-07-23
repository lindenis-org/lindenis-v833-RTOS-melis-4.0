/*
**********************************************************************************************************************
*                                                    ePOS
*                                   the Easy Portable/Player Operation System
*
*                                 (c) Copyright 2007-2008, Steven.ZGJ.China
*                                            All    Rights Reserved
*
* File    : devman.c
* By      : steven.ZGJ
* Version : V1.00
**********************************************************************************************************************
*/
#include "hwscdev.h"
#include <sys_device.h>
#include <sys_fsys.h>
#include <string.h>
#include <log.h>

static __dev_devop_t    hwscdev_ops;
static struct hwscdev   hwsc;

static __hdle   hwschandle = 0;
static __hdle   hMonitor = 0;

__s32 hwscdev_init(void)
{
    //register "hwsc" device
    hwschandle = esDEV_DevReg(DEV_CLASS_HWSC, DEV_NAME_HWSC, &hwscdev_ops, 0);
    if (!hwschandle)
    {
        __err("hwscdev registered Error!");
        return EPDK_FAIL;
    }

    //plug-in mointor
    esDEV_Plugin("\\drv\\monitor.drv", 0, 0, 1);

    //open monitor
    hMonitor = esFSYS_fopen("b:\\HWSC\\monitor", "rb+");
    if (!hMonitor)
    {
        __err("hwsc try to open monitor failed!");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32 hwscdev_exit(void)
{
    //close monitor
    if (hMonitor)
    {
        esFSYS_fclose((__hdle)hMonitor);
        hMonitor = 0;
    }

    //plugout monitor
    esDEV_Plugout("\\drv\\monitor.drv", 0);

    return esDEV_DevUnreg(hwschandle);
}


__hdle hwscdev_Open(void *open_arg, __u32 mode)
{
    hwsc.inuse++;
    return (__hdle)&hwsc;
}
__s32 hwscdev_Close(__hdle hDev)
{
    if (hwsc.inuse)
    {
        hwsc.inuse--;
        return EPDK_OK;
    }
    return EPDK_FAIL;
}

__u32 hwscdev_Read(void *pBuffer, __u32 size, __u32 n, __hdle hDev)
{
    return 0;
}
__u32 hwscdev_Write(const void *pBuffer, __u32 size, __u32 n, __hdle hDev)
{
    return 0;
}

__s32 hwscdev_Ioctrl(__hdle hDev, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    //send command to monitor
    if (hMonitor)
    {
        return esFSYS_fioctrl(hMonitor, Cmd, Aux, pBuffer);
    }

    return EPDK_FAIL;
}

static __dev_devop_t hwscdev_ops =
{
    hwscdev_Open,
    hwscdev_Close,
    hwscdev_Read,
    hwscdev_Write,
    hwscdev_Ioctrl
};

