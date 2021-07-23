/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_make.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-24
* Descript: Mini rom image make handing exteral header file.
* Update  : date                auther      ver     notes
*           2011-3-24 22:26:25  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __MINFS_MAKE_H__
#define __MINFS_MAKE_H__

typedef struct __MINFS_CONTEXT
{
    __u8   *pBuffer;
    __u32   BufferLen;
    __u32   CurDEntryOffset;
    __u32   CurDataOffset;
    __u32   DEntryLen;
    __u32   DataLen;
    __u32   ImageSize;
    __u8    ScanRoot[MAX_PATH];
    __u32   RootDirSize;
} __minfs_context_t;

__s32 MINFS_MakeImage(const char *pDir,
                      __minfs_context_t *pContext,
                      __minfs_config_t *pConfig);

#endif  //__MINFS_MAKE_H__
