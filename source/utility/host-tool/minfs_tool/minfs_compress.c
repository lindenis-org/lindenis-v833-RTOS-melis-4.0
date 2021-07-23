/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_compress.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-25
* Descript: Mini rom image file system compress handing functions.
* Update  : date                auther      ver     notes
*           2011-3-25 13:57:32  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "minfs_tool_i.h"
#include "lzma/LzmaLib.h"

__s32 MINFS_Compress(__u8 *pDst, __u32 *DstLen,
                     __u8 *pSrc, __u32 SrcLen,
                     __u8 *OutProps, __u32 *OutPropsSize)
{
    if (LzmaCompress(pDst, DstLen, pSrc, SrcLen, OutProps, OutPropsSize))
    {
        MSG("lzma compress data failed\n");
        return EPDK_FAIL;
    }
    if (*OutPropsSize != LZMA_PROPS_SIZE)
    {
        MSG("lzma compress props size error\n");
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

__s32 MINFS_Uncompress(__u8 *pDst, __u32 *DstLen, __u8 *pSrcData, __u32 *SrcLen)
{
    __u8   *pCompressData;

    //uncompress data
    pCompressData  = pSrcData + LZMA_PROPS_SIZE;
    (*SrcLen)     -= (LZMA_PROPS_SIZE);
    if (LzmaUncompress(pDst, DstLen, \
                       pCompressData, SrcLen, \
                       pSrcData, LZMA_PROPS_SIZE))
    {
        MSG("lzma uncompress data failed\n");
        return EPDK_FAIL;
    }
    return EPDK_OK;
}
