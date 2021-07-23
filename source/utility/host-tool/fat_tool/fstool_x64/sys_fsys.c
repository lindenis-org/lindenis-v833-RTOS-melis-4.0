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
#include "sys_fsys_i.h"

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
__u32 FSYS_Init(void)
{
    fsys_vfs_init();
    fsys_fatfs_init();      /* fat file system  */

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

__u32 FSYS_Exit(void)
{
    fsys_fatfs_exit();
    fsys_vfs_exit();
    return EPDK_OK;
}

