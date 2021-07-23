/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : dmspart.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: device management system partition.
* Update  : date                auther      ver     notes
*           2011-3-15 15:01:03  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "dmspart.h"
#include "fsys_libs.h"
#include <string.h>
#include <sys_device.h>
#include <sys_fsys.h>

/*
**********************************************************************************************************************
*             Local functions section
**********************************************************************************************************************
*/

/*
**********************************************************************************************************************
*
*             dmspart_read
*
  Description:
  FS internal function. Read sector from device.

  Parameters:
  pDriver     - Pointer to a device driver structure.
  Unit        - Unit number.
  Sector      - Sector to be read from the device.
  pBuffer     - Pointer to buffer for storing the data.

  Return value:
  ==0         - Sector has been read and copied to pBuffer.
  <0          - An error has occured.
**********************************************************************************************************************
*/

static __u32 dmspart_read(void *pData, __u32 Sector, __u32 N, __fsys_part_t *pPart)
{
    return 0;
}


/*
**********************************************************************************************************************
*
*             dmspart_write
*
*  Description:
*   FS internal function. Write sector to device.
*
*  Parameters:
*   pDriver     - Pointer to a device driver structure.
*   Sector      - Sector to be written to the device.
*   pBuffer     - Pointer to data to be stored.
*
*  Return value:
*   ==0         - Sector has been written to the device.
*   <0          - An error has occured.
**********************************************************************************************************************
*/
static __u32 dmspart_write(const void *pData, __u32 Sector, __u32 N, __fsys_part_t *pPart)
{
    return 0;
}

/*
**********************************************************************************************************************
*
*             dmspart_ioctl
*
  Description:
  FS internal function. Execute device command.

  Parameters:
  pPart       - Pointer to a partition driver structure.
  Cmd         - Command to be executed.
  Aux         - Parameter depending on command.
  pBuffer     - Pointer to a buffer used for the command.

  Return value:
  Command specific. In general a negative value means an error.
**********************************************************************************************************************
*/
static __s32 dmspart_ioctl(__fsys_part_t *pPart, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    __s32           x;
    __hdle          hDev;

    hDev = pPart->hDev;
    if (!hDev)
    {
        return EPDK_FAIL;
    }

    switch (Cmd)
    {
        case PART_IOC_USR_GETITEM:              /* 获取信息                     */
            esDEV_Lock(pPart->hNode);
            x = esDEV_Ioctl(hDev, DEV_IOC_USR_GET_DEVITEM, Aux, pBuffer);
            esDEV_Unlock(pPart->hNode);
            return x;
        case PART_IOC_USR_GETDEVCLSROOT:
            *((__u32 *)pBuffer) = (__u32)pPart->hPDPrivate;
            return EPDK_OK;
        case PART_IOC_USR_FLUSHCACHE:
            return EPDK_OK;
        default:
            return EPDK_FAIL;
    }
}

/*
**********************************************************************************************************************
*
*             dmspart_mount
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

static __s32 dmspart_mount(__fsys_part_t *pPart)
{
    __s32           x;
    __hdle          hDev;
    __hdle          hDevRoot;

    hDev = pPart->hDev;
    if (!hDev)
    {
        return EPDK_FAIL;
    }

    esDEV_Lock(pPart->hNode);

    x = esDEV_Ioctl(hDev, DEV_IOC_USR_GET_DEVROOT, 0, &hDevRoot);

    esDEV_Unlock(pPart->hNode);

    if (x == EPDK_OK)
    {
        pPart->hPDPrivate = hDevRoot;
    }
    else
    {
        pPart->hPDPrivate = 0;
    }

    return x;
}
/*
**********************************************************************************************************************
*
*             dmspart_unmount
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

static __s32 dmspart_unmount(__fsys_part_t *pPart, int force)
{
    __s32 res;

    res = deactivate_part(pPart, force);

    if (res == EPDK_OK || force)
    {
        pPart->status = FSYS_PARTSTATUS_DEAD;
    }

    return res;
}

/*
**********************************************************************************************************************
*
*             dmspart_identify
*
  Description:识别分区，如果hDev所指向的块设备为普通磁盘分区格式，那么返回EPDK_OK


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
static __u32 dmspart_identify(__hdle hDev)
{
    int res;
    char *className;

    res = esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_CLSNAME, 0, &className);

    if (res == EPDK_FAIL)
    {
        return 0;
    }
    if (strcmp(className, DEV_CLASS_DMS))
    {
        return 0;
    }

    return 1;
}

__fsys_pd_t dmspart_type =
{
    .name   = "dmsPart",
    .Ops    = {
        .read       = dmspart_read,
        .write      = dmspart_write,
        .ioctl      = dmspart_ioctl,
        .mount      = dmspart_mount,
        .unmount    = dmspart_unmount,
        .identify   = dmspart_identify,
        .fdisk      = 0
    }
};

/*
**********************************************************************************************************************
*             Global functions
**********************************************************************************************************************
*
* Functions here are global, although their names indicate a local
  scope. They should not be called by user application.
*/

/*
**********************************************************************************************************************
*
*             fsys_dmspart_init
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

__s32 fsys_dmspart_init(void)
{
    return esFSYS_pdreg((__hdle)&dmspart_type);
}

/*
**********************************************************************************************************************
*
*             fsys_dmspart_exit
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
__s32 fsys_dmspart_exit(void)
{
    return esFSYS_pdunreg((__hdle)&dmspart_type);
}


