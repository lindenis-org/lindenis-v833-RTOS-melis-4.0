/*
**********************************************************************************************************************
*                                                   ePOS
*                                  the Easy Portable/Player Operation System
*                                              fsys sub-system
*
*                                (c) Copyright 2007-2008, Steven.ZGJ.China
*                                           All Rights Reserved
*
* File    : fsys.c
* By      : steven.ZGJ
* Version : V1.00
**********************************************************************************************************************
*/
#include "sys_dev_i.h"
#include <kapi.h>
#include <log.h>

__hdle pDevSem;           /* for lock parts table */

/*
**********************************************************************************************************************
*
*             FS_Init
*
*  Description:
*  API function. Start the file system.
*
*  Parameters:
*  None.
*
*  Return value:
*  ==0         - File system has been started.
*  !=0         - An error has occured.
**********************************************************************************************************************
*/
__s32 dev_init(void)
{
    pDevSem = esKRNL_SemCreate(1);
    if (pDevSem == NULL)
    {
        __err("create sem failure.");
        return EPDK_FAIL;
    }

#ifdef CONFIG_LEGACY_DMS_DEV
    dmsdev_init();
#endif

#ifdef CONFIG_LEGACY_RAMDISK_DEV
    ramdiskdev_init();
#endif

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*             FS_Exit
*
*  Description:
*  API function. Stop the file system.
*
*  Parameters:
*  None.
*
*  Return value:
*  ==0         - File system has been stopped.
*  !=0         - An error has occured.
**********************************************************************************************************************
*/
__s32 dev_exit(void)
{
    __u32   x = EPDK_OK;

    ramdiskdev_exit();
    dmsdev_exit();

    return  x;
}

__s32 hwsc_init(void)
{
    return hwscdev_init();
}

__s32 hwsc_exit(void)
{
    return hwscdev_exit();
}
