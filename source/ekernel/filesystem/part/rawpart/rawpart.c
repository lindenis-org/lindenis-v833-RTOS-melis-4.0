/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : rawpart.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: raw partition system.
* Update  : date                auther      ver     notes
*           2011-3-15 15:02:30  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "rawpart.h"
#include "fsys_debug.h"

#define pPDPrivate  ((__fsys_rawpart_p_t *)(pPart->hPDPrivate))

/*
**********************************************************************************************************************
*
*             part_read
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

__u32 rawpart_read(void *pData, __u32 Sector, __u32 N, __fsys_part_t *pPart)
{
    __s32 x;
    __hdle hDev;

    if (!pPart)
    {
        return 0;
    }

    hDev = pPart->hDev;
    if (hDev)
    {
        esDEV_Lock(pPart->hNode);
        x = esDEV_Read(pData, Sector, N, hDev);
        esDEV_Unlock(pPart->hNode);
        return  x;
    }
    return 0;
}


/*
**********************************************************************************************************************
*
*             part_write
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
__u32 rawpart_write(const void *pData, __u32 Sector, __u32 N, __fsys_part_t *pPart)
{
    __s32 x;
    __hdle hDev;

    if (!pPart)
    {
        return 0;
    }

    hDev = pPart->hDev;
    if (hDev)
    {
        esDEV_Lock(pPart->hNode);
        x = esDEV_Write(pData, Sector, N, hDev);
        esDEV_Unlock(pPart->hNode);
        return  x;
    }
    return 0;
}

/*
**********************************************************************************************************************
*
*             part_ioctl
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

__s32  rawpart_ioctl(__fsys_part_t *pPart, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    __s32  x;
    __hdle hDev;

    if (!pPart)
    {
        return 0;
    }

    hDev = pPart->hDev;
    if (!hDev)
    {
        return EPDK_FAIL;
    }

    switch (Cmd)
    {
        case PART_IOC_USR_FLUSHCACHE:
        {
            esDEV_Lock(pPart->hNode);
            esDEV_Ioctl(pPart->hDev, DEV_IOC_USR_FLUSH_CACHE, 0, NULL);
            esDEV_Unlock(pPart->hNode);
            break;
        }

        case PART_IOC_USR_GETPARTSIZE:
        {
            *((__s64 *)pBuffer) = pPDPrivate->partseccnt;
            break;
        }

        case PART_IOC_USR_GETSCTSIZE:
        {
            *((__u32 *)pBuffer) = pPDPrivate->partsecsize;
            break;
        }
        /***********************************old******************/
        case FSYS_PART_CMD_GET_STATUS:              /* 获取状态信息                 */
        {
            return 0;
        }

        case FSYS_PART_CMD_GET_INFO:                /* 获取信息                     */
        {
            esDEV_Lock(pPart->hNode);
            x = esDEV_Ioctl(hDev, DEV_CMD_GET_INFO, Aux, pBuffer);
            esDEV_Unlock(pPart->hNode);
            return x;
        }

        case FSYS_PART_CMD_INC_BUSYCNT:             /*  */
        {
            break;
        }

        case FSYS_PART_CMD_DEC_BUSYCNT:             /*  */
        {
            break;
        }

        case FSYS_PART_CMD_FLUSH_CACHE:
        {
            esDEV_Lock(pPart->hNode);
            esDEV_Ioctl(pPart->hDev, DEV_IOC_USR_FLUSH_CACHE, 0, NULL);
            esDEV_Unlock(pPart->hNode);
            break;
        }

        case FSYS_PART_CMD_GET_PARTSIZE:
        {
            esDEV_Lock(pPart->hNode);
            *((__s64 *)pBuffer) = pPDPrivate->partseccnt;
            esDEV_Unlock(pPart->hNode);
            break;
        }
        /***********************************old******************/
        default:
        {
            return EPDK_FAIL;
        }
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*             npart_mount
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

__s32  rawpart_mount(__fsys_part_t *pPart)
{
    __dev_blkinfo_t blkinf;


    /* 分配私有的数据空间       */
    pPart->hPDPrivate = (__hdle)malloc(sizeof(__fsys_rawpart_p_t));
    if (!(pPart->hPDPrivate))
    {
        fs_log_info("mount part fail!\n");
        return EPDK_FAIL;
    }

    esDEV_Lock(pPart->hNode);

    /* 计算出分区的信息         */
    esDEV_Ioctl(pPart->hDev, DEV_CMD_GET_INFO, 0, &blkinf);
    pPDPrivate->partaddr    = 0;
    pPDPrivate->partseccnt  = blkinf.partsize;
    pPDPrivate->partsecsize = blkinf.secsize;

    esDEV_Unlock(pPart->hNode);

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*             npart_unmount
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

__s32 rawpart_unmount(__fsys_part_t *pPart, int force)
{
    __s32  res;

    res = deactivate_part(pPart, force);
    if (res == EPDK_OK || force)
    {
        free((void *)pPart->hPDPrivate);
    }

    return res;
}
/*
**********************************************************************************************************************
*
*             rawpart_identify
*
  Description:识别分区，如果hDev所指向的块设备为普通磁盘分区格式，那么返回EPDK_OK


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
__u32 rawpart_identify(__hdle hDev)
{
    int res;
    char *className;

    res = esDEV_Ioctl(hDev, DEV_IOC_SYS_GET_CLSNAME, 0, &className);
    if (res == EPDK_FAIL)
    {
        return 0;
    }
    if (!strcmp(className, DEV_CLASS_DISK))
    {
        return 1;
    }

    return 0;
}
/*
**********************************************************************************************************************
*
*             rawpart_fdisk
*
  Description:分区


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/
__s32 rawpart_fdisk(__hdle hDev)
{
    return EPDK_FAIL;
}

__fsys_pd_t rawpart_type =
{
    .name   = "rawpart",
    .Ops    = {
        .read       = rawpart_read,
        .write      = rawpart_write,
        .ioctl      = rawpart_ioctl,
        .mount      = rawpart_mount,
        .unmount    = rawpart_unmount,
        .identify   = rawpart_identify,
        .fdisk      = rawpart_fdisk,
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
*             fsys_npart_init
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

__s32 fsys_rawpart_init(void)
{
    return esFSYS_pdreg((__hdle)&rawpart_type);
}

/*
**********************************************************************************************************************
*
*             fsys_npart_exit
*
  Description:


  Parameters:

  Return value:
    EPDK_OK/EPDK_FAIL
**********************************************************************************************************************
*/

__s32 fsys_rawpart_exit(void)
{
    return esFSYS_pdunreg((__hdle)&rawpart_type);
}



