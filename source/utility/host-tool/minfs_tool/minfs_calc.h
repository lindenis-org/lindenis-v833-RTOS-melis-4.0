/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_calc.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-23
* Descript: Mini rom image parameters calculator exteral header file.
* Update  : date                auther      ver     notes
*           2011-3-23 22:25:40  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#ifndef __MINFS_CALC_H__
#define __MINFS_CALC_H__

typedef struct __MINFS_PARA
{
    __bool  FDataCalcEn;
    __u32   RootDEntryNum;
    __u32   RootDEntryLen;
    __u32   DEnrtyNum;
    __u32   DEntryLen;
    __u32   FDataLen;
} __minfs_para_t;

__s32 MINFS_CalcMFSImagePara(const char *pDir,
                             __minfs_para_t *pMFSPara,
                             __minfs_config_t *pConfig);

#endif  //__MINFS_CALC_H__
