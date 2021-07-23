/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_tool.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-22
* Descript: Mini rom image file system structure.
* Update  : date                auther      ver     notes
*           2011-3-22 18:56:16  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include    "minfs_tool_i.h"

__s32 MINFS_SaveImageToFile(const char *pFile, __minfs_context_t *pContext)
{
    FILE    *hFile;
    __u32    WriteNum;

    hFile = fopen(pFile, "wb+");
    if (hFile == NULL)
    {
        MSG("create image file [%s] failed\n", pFile);
        return EPDK_FAIL;
    }
    WriteNum = fwrite(pContext->pBuffer, 1, pContext->ImageSize, hFile);
    if (WriteNum != pContext->ImageSize)
    {
        MSG("write image file [%s] failed\n", pFile);
        fclose(hFile);
        return EPDK_FAIL;
    }

    fclose(hFile);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     CREATE ONE MINFS VOLUME IMAGE
*********************************************************************************************************
*/
__s32 MINFS_Create(const char *pDir, const char *pFile, const char *pConfig)
{
    __s32               ret = EPDK_OK;
    __minfs_context_t   Context;
    __minfs_hdr_t      *pMinfsHdr;
    __minfs_para_t      MFSImagePara;
    __u32               FDataOffset;
    __minfs_config_t    ImageConfig;

    if ((pDir == NULL) || (pFile == NULL))
    {
        return EPDK_FAIL;
    }
    //parser config file first
    memset(&ImageConfig, 0, sizeof(__minfs_config_t));
    if (MINFS_ParseConfigFile(pConfig, &ImageConfig) != EPDK_OK)
    {
        MSG("parser config file failed\n");
        return EPDK_FAIL;
    }

    //initialize minfs image create context
    memset(&Context, 0, sizeof(__minfs_context_t));

    //initialize minfs image parameters
    memset(&MFSImagePara, 0, sizeof(__minfs_para_t));

    //initialize scan root
    strcpy(Context.ScanRoot, pDir);

    //calculate the DEntry area space length and DEntry number,
    //the physical layout of minfs: MINFS_HDR + DEntry_Area + FileData_Area,
    //this can improve the accessed performance for melis system,
    //by sunny at 2011-3-23 20:10:42.
    ret = MINFS_CalcMFSImagePara(pDir, &MFSImagePara, &ImageConfig);
    if (ret != EPDK_OK)
    {
        MSG("calculate the DEntry information failed\n");
        return EPDK_FAIL;
    }
    //calc file data offset
    FDataOffset = MINFS_ALIGN((MINFS_HDR_LEN + MFSImagePara.DEntryLen), \
                              MINFS_SECTOR_LEN);

    //check config image size enough or not
    if (FDataOffset > ImageConfig.Size)
    {
        MSG("image file have no free space\n");
        return EPDK_FAIL;
    }

    //allocate buffer for minfs image,
    //minfs create is processed within ram
    Context.pBuffer = (__u8 *)malloc(ImageConfig.Size);
    if (Context.pBuffer == NULL)
    {
        return EPDK_FAIL;
    }
    Context.BufferLen = ImageConfig.Size;
    memset(Context.pBuffer, 0, Context.BufferLen);

    //make minfs image
    Context.ImageSize       = ImageConfig.Size;
    Context.CurDEntryOffset = MINFS_HDR_LEN;
    Context.CurDataOffset   = FDataOffset;
    Context.RootDirSize     = MFSImagePara.RootDEntryLen;

    ret = MINFS_MakeImage(pDir, &Context, &ImageConfig);
    if (ret != EPDK_OK)
    {
        MSG("make minfs image failed\n");
        free(Context.pBuffer);
        return EPDK_FAIL;
    }

    //calc file data area size
    MFSImagePara.FDataLen = Context.CurDataOffset - FDataOffset;

    //update minfs header
    pMinfsHdr = (__minfs_hdr_t *)(Context.pBuffer);
    memset(pMinfsHdr, 0, MINFS_HDR_LEN);
    memcpy(pMinfsHdr->Magic, "MINFS", 5);
    pMinfsHdr->Magic[5]     = 0;
    pMinfsHdr->Version      = MINFS_VERSION;
    pMinfsHdr->RootDirOffset = MINFS_HDR_LEN;
    pMinfsHdr->RootDirSize  = MFSImagePara.RootDEntryLen;
    pMinfsHdr->DEntryNum    = MFSImagePara.DEnrtyNum;
    pMinfsHdr->DEntryLen    = MFSImagePara.DEntryLen;
    pMinfsHdr->FDataLen     = MFSImagePara.FDataLen;
    pMinfsHdr->Size         = ImageConfig.Size;

    //save image buffer to file
    ret = MINFS_SaveImageToFile(pFile, &Context);
    if (ret != EPDK_OK)
    {
        MSG("make minfs image failed\n");
        free(Context.pBuffer);
        return EPDK_FAIL;
    }

    //dump minfs image size information
    MSG("-------------------------------------------------------------\n");
    MSG("make minfs image file [%s] succeeded\n", pFile);
    MSG("image config size  : [%8d]\n", ImageConfig.Size);
    MSG("image used   size  : [%8d]\n", Context.CurDataOffset);
    MSG("DEntry count       : [%8d]\n", MFSImagePara.DEnrtyNum);
    MSG("DEntry length      : [%8d]\n", MFSImagePara.DEntryLen);
    MSG("FData  length      : [%8d]\n", MFSImagePara.FDataLen);
    MSG("-------------------------------------------------------------\n");

    //create minfs image succeeded
    return EPDK_OK;
}
