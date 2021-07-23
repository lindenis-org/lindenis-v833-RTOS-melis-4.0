/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_compress.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-24
* Descript: Mini rom image file system compress interface.
* Update  : date                auther      ver     notes
*           2011-3-24 16:38:01  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#ifndef __MINFS_COMPRESS_H__
#define __MINFS_COMPRESS_H__

#define MINFS_MAX_COMPRESS_PROPS_LEN    128
#define MINFS_MAX_EXPAND_RATIO          4

__s32 MINFS_Compress(__u8 *pDst, __u32 *DstLen,
                     __u8 *pSrc, __u32 SrcLen,
                     __u8 *OutProps, __u32 *OutPropsSize);
__s32 MINFS_Uncompress(__u8 *pDst, __u32 *DstLen,
                       __u8 *pSrcData, __u32 *SrcLen);

#endif  // __MINFS_COMPRESS_H__
