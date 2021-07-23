/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Flash Module
*
*                                    (c) Copyright 2010-2014, wfsun China
*                                             All Rights Reserved
*
* File    : mod_slib_i.h
* By      : wfsun
* Version : v1.0
* Date    : 2011-1-18
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#ifndef __MOD_SLIB_I_H__
#define __MOD_SLIB_I_H__

#include <typedef.h>
#include <module/mod_defs.h>

__s32 SLIB_MInit(void);
__s32 SLIB_MExit(void);
__mp *SLIB_MOpen(__u32 mid, __u32 mode);
__s32 SLIB_MClose(__mp *mp);
__u32 SLIB_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 SLIB_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
__s32 SLIB_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

#endif  //__MOD_SLIB_I_H__

