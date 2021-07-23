/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_make.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-24
* Descript: Mini rom image make handing fucntions.
* Update  : date                auther      ver     notes
*           2011-3-24 22:26:25  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "minfs_tool_i.h"

typedef void *HANDLE;
#define INVALID_HANDLE_VALUE NULL

__minfs_dentry_t *MINFS_GetDEntryByPath(const char *pFullPath, __minfs_context_t *pContext)
{
    __minfs_dentry_t *pDEntry;
    char             *pMFSPath;
    char              MFSName[MAX_PATH];
    char             *pMFSName;
    __bool            FindFlag;
    __u32             DirSize;
    __u32             tmpLen;

    //split full path into scan root and minfs path,
    //full path = ScanRoot + minfsPath
    pMFSPath = (char *)(pFullPath + strlen(pContext->ScanRoot));

    //seach the target dentry by path,
    //we just seach within the directory have been created,
    //walk form minfs rooot directory.
    tmpLen   = 0;
    FindFlag = 0;
    DirSize  = pContext->RootDirSize;
    pDEntry  = (__minfs_dentry_t *)(pContext->pBuffer + MINFS_HDR_LEN);
    __u32 offset = MINFS_HDR_LEN;
    while ((__u32)(pDEntry) < (__u32)(pContext->pBuffer + pContext->CurDEntryOffset))
    {
        while ((*pMFSPath == '/') && (*pMFSPath != '\0'))
        {
            pMFSPath++;
        }

        //extract one dentry from minfs path
        pMFSName = MFSName;
        while ((*pMFSPath != '/') && (*pMFSPath != '\0'))
        {
            *pMFSName++ = *pMFSPath++;
        }
        *pMFSName = '\0';

        //seach current dentry
        while (tmpLen < DirSize)
        {
            if (memcmp(MFSName, pDEntry->MetaData, pDEntry->NameLen) == 0)
            {
                //one dentry item find
                FindFlag = 1;
                break;
            }
            //next dentry
            tmpLen             += (pDEntry->RecLen);

            //pDEntry = (__minfs_dentry_t *)len;
            //((__u32)pDEntry) = pDEntry->RecLen+((__u32)pDEntry);
            pDEntry = (__minfs_dentry_t *)(pContext->pBuffer + offset + tmpLen);
            //printf("tmpLen:%d\n",tmpLen);

        }
        if (FindFlag)
        {
            if (*pMFSPath == '\0')
            {
                //target dentry finded
                return pDEntry;
            }
            else
            {
                //just find internal dentry,
                //continue to seach the last target dentry.
                tmpLen = 0;

                DirSize = pDEntry->Size;
                //printf("DirSize=%d\n",DirSize);
                offset = pDEntry->Offset;
                pDEntry = (__minfs_dentry_t *)(pContext->pBuffer + pDEntry->Offset);
            }
        }
        else
        {
            //seach failed
            break;
        }
    }
    //seach target dentry failed
    return NULL;
}

__s32 MINFS_CompressData(__u8 *pDstBuffer, __u32 *Dstlen, __u8 *pSrcBuffer, __u32 SrcLen)
{
    __u8    *pCompressData;
    __u8     OutProps[MINFS_MAX_COMPRESS_PROPS_LEN];
    __u32    OutPropsLen;
    __u32    CompressLen;

    //allocate buffer to store compressed data,
    //l can't calc the compressed data length.
    //l think MINFS_MAX_COMPRESS_RATIO * FileLen is enough,
    //maybe it is not safe,
    //if you kown good solution for this problem,
    //please tell me.
    //by sunny at 2011-3-24 17:07:06.
    CompressLen   = MINFS_MAX_EXPAND_RATIO * SrcLen;
    pCompressData = (__u8 *)malloc(CompressLen);
    if (pCompressData == NULL)
    {
        MSG("allocate buffer for compress data failed\n");
        return EPDK_FAIL;
    }
    //initialize props data
    memset(OutProps, 0, MINFS_MAX_COMPRESS_PROPS_LEN);
    OutPropsLen = MINFS_MAX_COMPRESS_PROPS_LEN;

    //compress minfs file data
    if (MINFS_Compress(pCompressData, &CompressLen,
                       pSrcBuffer, SrcLen,
                       OutProps, &OutPropsLen) != EPDK_OK)
    {
        MSG("compress data failed\n");
        free(pCompressData);
        return EPDK_FAIL;
    }
    if (OutPropsLen > MINFS_MAX_COMPRESS_PROPS_LEN)
    {
        MSG("the compressed algorithmic property is too long\n");
        free(pCompressData);
        return EPDK_FAIL;
    }

    //check dest buffer length enough or not
    if ((OutPropsLen + CompressLen) > (*Dstlen))
    {
        MSG("compress dest buffer not enough\n");
        free(pCompressData);
        return EPDK_FAIL;
    }

    //store property data and compress data to buffer.
    memcpy(pDstBuffer, OutProps, OutPropsLen);
    memcpy(pDstBuffer + OutPropsLen, pCompressData, CompressLen);

    //calc dest file length, and update dentry size and parameter attribute
    (*Dstlen) = OutPropsLen + CompressLen;

    //release compress buffer
    free(pCompressData);

    return EPDK_OK;
}

__s32 MINFS_LoadFileData(const char *pFullPath,
                         __minfs_dentry_t *pDEntry,
                         __minfs_context_t *pContext,
                         __minfs_config_t *pConfig)
{
    __u8            *pFileData;
    __u32            SrcFileLen;
    __u32            MFSFileLen;
    FILE            *hFile;
    __file_para_t    FilePara;
    __u8            *pBuffer;

    //parse this file config information
    if (MINFS_ParseFileConfigPara(pConfig, pFullPath, &FilePara) != EPDK_OK)
    {
        MSG("parse file [%s] config parameters failed\n", pFullPath);
        return EPDK_FAIL;
    }

    //open source file for load data
    hFile = fopen(pFullPath, "rb");
    if (hFile == NULL)
    {
        MSG("MINFS_LoadFileData : open file [%s] failed\n", pFullPath);
        return EPDK_FAIL;
    }

    //get file length
    fseek(hFile, 0, SEEK_END);
    SrcFileLen = (__u32)ftell(hFile);
    fseek(hFile, 0, SEEK_SET);

    //allocate buffer to store file data
    pFileData = (__u8 *)malloc(SrcFileLen);
    if (pFileData == NULL)
    {
        MSG("allocate for file data buffer failed\n");
        fclose(hFile);
        return EPDK_FAIL;
    }
    //load file data to buffer
    fread(pFileData, 1, SrcFileLen, hFile);
    fclose(hFile);

    //allocate buffer from minfs image for this file
    pBuffer = pContext->pBuffer + pContext->CurDataOffset;
    if (FilePara.CompressON)
    {
        //this file should compressed or not
        MFSFileLen = pContext->ImageSize - pContext->CurDataOffset;
        if (MINFS_CompressData(pBuffer, &MFSFileLen,
                               pFileData, SrcFileLen)
                != EPDK_OK)
        {
            MSG("image file have no free space\n");
            return EPDK_FAIL;
        }
        pDEntry->Attribute |= MINFS_ATTR_COMPRESS;
    }
    else
    {
        //check image file have free space to use
        if ((pContext->CurDataOffset + SrcFileLen) > pContext->ImageSize)
        {
            MSG("image file have no free space\n");
            return EPDK_FAIL;
        }
        //copy file data to minfs image directly.
        MFSFileLen = SrcFileLen;
        memcpy(pBuffer, pFileData, MFSFileLen);
    }

    //update dentry size parameter
    pDEntry->Size = MFSFileLen;
    pDEntry->UnPackSize = SrcFileLen;

    //adjust minfs image context parameters
    pContext->CurDataOffset += MINFS_ALIGN(pDEntry->Size, MINFS_DATA_ALIGN);
    pContext->DataLen       += MINFS_ALIGN(pDEntry->Size, MINFS_DATA_ALIGN);

    //release file buffer
    free(pFileData);

    return EPDK_OK;
}

__s32 MINFS_ProcessFile(const char *pFullPath,
                        const char *pFileName,
                        __u32 *FEntryLen,
                        __minfs_context_t *pContext,
                        __minfs_config_t *pImageConfig)
{
    __minfs_dentry_t *pDEntry;

    //allocate a dentry for this file
    pDEntry = (__minfs_dentry_t *)(pContext->pBuffer + pContext->CurDEntryOffset);

    //allocate current data offset to file data area,
    //update file name relative parameters.
    pDEntry->Offset    = pContext->CurDataOffset;
    pDEntry->Attribute = 0;
    pDEntry->NameLen   = strlen(pFileName);
    pDEntry->RecLen    = MINFS_DENTRY_LEN + \
                         MINFS_ALIGN(pDEntry->NameLen, MINFS_NAME_ALIGN);
    memcpy(pDEntry->MetaData, pFileName, pDEntry->NameLen);

    //allocate this file data area
    if (MINFS_ValidModuleFile(pFullPath))
    {
        //valid module file for melis system
        if (MINFS_LoadModuleFile(pFullPath, pDEntry, \
                                 pContext, pImageConfig) != EPDK_OK)
        {
            MSG("load module file [%s] failed\n", pFullPath);
            return EPDK_FAIL;
        }
    }
    else
    {
        //normal file
        if (MINFS_LoadFileData(pFullPath, pDEntry, \
                               pContext, pImageConfig) != EPDK_OK)
        {
            MSG("load file [%s] data failed\n", pFullPath);
            return EPDK_FAIL;
        }
    }

    //dentry length should align by minfs spec
    pDEntry->RecLen  = MINFS_ALIGN(pDEntry->RecLen, MINFS_DENTRY_ALIGN);

    //return file entry record length
    (*FEntryLen) = pDEntry->RecLen;

    //this file process succeeded
    return EPDK_OK;
}

__s32 MINFS_ProcessDir(const char *pDir,
                       __minfs_context_t *pContext,
                       __minfs_config_t *pImageConfig)
{
    __u32               ret = EPDK_OK;
    char                fullpath[MAX_PATH];
    __minfs_dentry_t   *pDirEntry;
    __minfs_dentry_t   *pCurEntry;
    __u32               FEntryLen;
    __u32               DEntrySize;

    //get the directory entry pointer of pDir,
    //if pDir is the root, we no need do any thing.
    pDirEntry = NULL;
    if (strcasecmp(pDir, pContext->ScanRoot) != 0)
    {
        //not root directory
        pDirEntry = MINFS_GetDEntryByPath(pDir, pContext);
        if (pDirEntry == NULL)
        {
            MSG("get the directory entry [%s] failed\n", pDir);
            return EPDK_FAIL;
        }
    }

    if (pDirEntry)
    {
        //allocate current dentry offset to pDir offset.
        pDirEntry->Offset = pContext->CurDEntryOffset;
    }

    //scan the first level of directory
    DEntrySize = 0;

    char path[512];
    struct dirent *filename;//readdir 的返回类型
    DIR *dir;//血的教训阿，不要随便把变量就设成全局变量。。。。
    dir = opendir(pDir);
    if (dir == NULL)
    {
        printf("open dir %s error!\n", pDir);
        exit(1);
    }

    while ((filename = readdir(dir)) != NULL)
    {
        //目录结构下面问什么会有两个.和..的目录？ 跳过着两个目录
        if (!strcmp(filename->d_name, ".") || !strcmp(filename->d_name, ".."))
            continue;

        //非常好用的一个函数，比什么字符串拼接什么的来的快的多
        sprintf(path, "%s/%s", pDir, filename->d_name);

        struct stat s;
        lstat(path, &s);
        if (S_ISDIR(s.st_mode))
        {
            pCurEntry            = (__minfs_dentry_t *)(pContext->pBuffer + \
                                   pContext->CurDEntryOffset);
            pCurEntry->Offset    = 0;
            pCurEntry->Size      = 0;
            pCurEntry->Attribute = MINFS_ATTR_DIR;
            pCurEntry->NameLen   = strlen(filename->d_name);
            pCurEntry->ExtentLen = 0;
            pCurEntry->RecLen    = MINFS_DENTRY_LEN + \
                                   MINFS_ALIGN((pCurEntry->NameLen), \
                                               MINFS_NAME_ALIGN);
            pCurEntry->RecLen    = MINFS_ALIGN((pCurEntry->RecLen), \
                                               MINFS_DENTRY_ALIGN);
            memcpy(pCurEntry->MetaData, filename->d_name, pCurEntry->NameLen);

            //adjust context dentry offset and DEntrySize
            pContext->CurDEntryOffset += pCurEntry->RecLen;
            pContext->DEntryLen       += pCurEntry->RecLen;
            DEntrySize                += pCurEntry->RecLen;
        }
        else if (S_ISREG(s.st_mode))
        {
            ret = MINFS_ProcessFile(path,
                                    filename->d_name,
                                    &FEntryLen,
                                    pContext,
                                    pImageConfig);
            if (ret != EPDK_OK)
            {
                MSG("make image failed when file [%s]\n", path);
                ret = EPDK_FAIL;
                break;
            }

            //adjust context dentry offset and DEntrySize
            pContext->CurDEntryOffset += FEntryLen;
            pContext->DEntryLen       += FEntryLen;
            DEntrySize                += FEntryLen;
        }
    }
    closedir(dir);

    if (pDirEntry)
    {
        //update pDir entry size
        pDirEntry->Size = DEntrySize;
    }

    return ret;
}

__s32 MINFS_MakeImage(const char *pDir,
                      __minfs_context_t *pContext,
                      __minfs_config_t *pConfig)
{
    __u32 ret = EPDK_OK;

    ret = MINFS_ProcessDir(pDir, pContext, pConfig);
    if (ret != EPDK_OK)
    {
        MSG("process diretory [%s] failed\n", pDir);
        return __LINE__;
    }

    char path[512];
    struct dirent *filename;//readdir 的返回类型
    DIR *dir;//血的教训阿，不要随便把变量就设成全局变量。。。。
    dir = opendir(pDir);
    if (dir == NULL)
    {
        printf("open dir %s error!\n", pDir);
        exit(1);
    }

    while ((filename = readdir(dir)) != NULL)
    {
        //目录结构下面问什么会有两个.和..的目录？ 跳过着两个目录
        if (!strcmp(filename->d_name, ".") || !strcmp(filename->d_name, ".."))
            continue;

        //非常好用的一个函数，比什么字符串拼接什么的来的快的多
        sprintf(path, "%s/%s", pDir, filename->d_name);

        struct stat s;
        lstat(path, &s);

        if (S_ISDIR(s.st_mode))
        {
            ret = MINFS_MakeImage(path, pContext, pConfig);
            if (ret != EPDK_OK)
            {
                MSG("make image failed when directory [%s]\n", path);
                break;
            }

        }
    }
    closedir(dir);
    return ret;
}
