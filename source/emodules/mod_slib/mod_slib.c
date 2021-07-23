/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               slib Module
*
*                                    (c) Copyright 2006-2010, sunny China
*                                             All Rights Reserved
*
* File    : mod_slib.c
* By      : sunny
* Version : v1.0
* Date    : 2011-4-30 14:29:53
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/

#include "mod_slib.h"
#include <module/mod_defs.h>
#include <log.h>

/*
****************************************************************************************************
*
*                           SLIB_MInit
*
*  Description:
*       SLIB_MInit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 SLIB_MInit(void)
{
    return EPDK_OK;
}

/*
****************************************************************************************************
*
*                           SLIB_MExit
*
*  Description:
*       SLIB_MExit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 SLIB_MExit(void)
{
    return EPDK_OK;
}

/*
****************************************************************************************************
*
*                           SLIB_MOpen
*
*  Description:
*       SLIB_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__mp *SLIB_MOpen(__u32 mid, __u32 mod)
{
    return NULL;
}

/*
****************************************************************************************************
*
*             SLIB_MClose
*
*  Description:
*       SLIB_MClose
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 SLIB_MClose(__mp *mp)
{
    return EPDK_OK;
}

/*
****************************************************************************************************
*
*             SLIB_MRead
*
*  Description:
*       SLIB_MRead
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 SLIB_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

/*
****************************************************************************************************
*
*             SLIB_MWrite
*
*  Description:
*       SLIB_MWrite
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 SLIB_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

/*
****************************************************************************************************
*
*             SLIB_MIoctrl
*
*  Description:
*       SLIB_MIoctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 SLIB_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    /* no ioctl command now */
    return EPDK_OK;
}
