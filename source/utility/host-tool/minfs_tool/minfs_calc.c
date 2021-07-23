/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_calc.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-23
* Descript: Mini rom image parameters calculator.
* Update  : date                auther      ver     notes
*           2011-3-23 22:25:40  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "minfs_tool_i.h"
typedef void *HANDLE;

__s32 MINFS_CalcCompressedFileLen(const char *pFullPath, __u32 *FDataLen)
{
    __u8            *pFileData;
    __u32            FileLen;
    FILE            *hFile;
    __u8            *pCompressData;
    __u8             OutProps[MINFS_MAX_COMPRESS_PROPS_LEN];
    __u32            OutPropsLen;
    __u32            CompressLen;

    //open source file for load data
    hFile = fopen(pFullPath, "rb");
    if (hFile == NULL)
    {
        MSG("MINFS_CalcCompressedFileLen : open file [%s] failed\n", pFullPath);
        return EPDK_FAIL;
    }

    //get file length
    fseek(hFile, 0, SEEK_END);
    FileLen = (__u32)ftell(hFile);
    fseek(hFile, 0, SEEK_SET);

    //allocate buffer to store file data
    pFileData = (__u8 *)malloc(FileLen);
    if (pFileData == NULL)
    {
        MSG("allocate for file data buffer failed\n");
        fclose(hFile);
        return EPDK_FAIL;
    }

    //load file data to buffer
    fread(pFileData, 1, FileLen, hFile);

    //close file handle
    fclose(hFile);

    //compress file data to get compressed data length
    pCompressData = (__u8 *)malloc(MINFS_MAX_EXPAND_RATIO * FileLen);
    if (pCompressData == NULL)
    {
        MSG("allocate buffer for compress data failed\n");
        free(pFileData);
        return EPDK_FAIL;
    }
    memset(OutProps, 0, MINFS_MAX_COMPRESS_PROPS_LEN);

    OutPropsLen = MINFS_MAX_COMPRESS_PROPS_LEN;

    //compress minfs file data
    if (MINFS_Compress(pCompressData, &CompressLen,
                       pFileData, FileLen,
                       OutProps, &OutPropsLen) != EPDK_OK)
    {
        MSG("compress file [%s] data failed\n", pFullPath);
        free(pFileData);
        free(pCompressData);
        return EPDK_FAIL;
    }
    if (OutPropsLen > MINFS_MAX_COMPRESS_PROPS_LEN)
    {
        MSG("the compressed algorithmic property is too long\n");
        free(pFileData);
        free(pCompressData);
        return EPDK_FAIL;
    }

    //calc compressed file length = CompressLen + OutPropsLen
    (*FDataLen) = OutPropsLen + CompressLen;

    //release file buffer and compress buffer
    free(pFileData);
    free(pCompressData);

    return EPDK_OK;
}

__s32 MINFS_CalcMFSFile(const char *pFullPath,
                        const char *pFileName,
                        __minfs_para_t *pMFSPara,
                        __minfs_config_t *pConfig)
{
    __file_para_t    FilePara;
    __u32            FDataLen;
    __u32            NameLen;
    __u32            RecLen;

    //parse this file config information
    if (MINFS_ParseFileConfigPara(pConfig, pFullPath, &FilePara) != EPDK_OK)
    {
        MSG("parse file [%s] config parameters failed\n", pFullPath);
        return EPDK_FAIL;
    }

    //calculate dentry name length
    NameLen = strlen(pFileName);
    NameLen = MINFS_ALIGN(NameLen, MINFS_NAME_ALIGN);

    //calculate dentry record length
    RecLen  = MINFS_DENTRY_LEN + NameLen;

    //check this file is a valid melis module file or not
    if (MINFS_ValidModuleFile(pFullPath))
    {
        __u32 ExtentLen;

        //valid module file of melis system
        if (pMFSPara->FDataCalcEn)
        {
            if (MINFS_CalcMFSModuleFile(pFullPath, \
                                        &ExtentLen, &FDataLen, pConfig) != EPDK_OK)
            {
                MSG("calculate module file [%s] "\
                    "inoformation failed\n", pFullPath);
                return EPDK_FAIL;
            }
        }
        else
        {
            //no need to calcute file data length
            if (MINFS_CalcMFSModuleFile(pFullPath, \
                                        &ExtentLen, NULL, pConfig) != EPDK_OK)
            {
                MSG("calculate module file [%s] "\
                    "inoformation failed\n", pFullPath);
                return EPDK_FAIL;
            }
            FDataLen = 0;
        }
        RecLen += ExtentLen;
    }
    else
    {
        if (pMFSPara->FDataCalcEn)
        {
            //calculate file data length
            if (FilePara.CompressON)
            {
                if (MINFS_CalcCompressedFileLen(pFullPath, &FDataLen) \
                        != EPDK_OK)
                {
                    MSG("calculate file [%s] compressed "\
                        "data length failed\n", pFullPath);
                    return EPDK_FAIL;
                }
            }
            else
            {
                FILE *hFile;

                //get file length directly
                hFile = fopen(pFullPath, "rb");
                if (hFile == NULL)
                {
                    MSG("MINFS_CalcMFSFile:open file [%s] failed\n", pFullPath);
                    return EPDK_FAIL;
                }
                fseek(hFile, 0, SEEK_END);
                FDataLen = (__u32)ftell(hFile);
                fseek(hFile, 0, SEEK_SET);
                fclose(hFile);
            }
        }
        else
        {
            //no need to calcute file data area length
            FDataLen = 0;
        }
    }

    //dentry record length should aligned by minfs spec
    RecLen   = MINFS_ALIGN(RecLen, MINFS_DENTRY_ALIGN);
    FDataLen = MINFS_ALIGN(FDataLen, MINFS_DATA_ALIGN);

    //update minfs image paraters
    pMFSPara->DEntryLen += RecLen;
    pMFSPara->DEnrtyNum += 1;
    pMFSPara->FDataLen  += FDataLen;

    //this file process succeeded
    return EPDK_OK;
}

__s32 MINFS_CalcMFSDir(const char *pDir,
                       __minfs_para_t *pMFSPara,
                       __minfs_config_t *pConfig)
{
    __u32               ret = EPDK_OK;
    __u32               NameLen;
    __u32               RecLen;

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
            NameLen = strlen(filename->d_name);
            NameLen = MINFS_ALIGN(NameLen, MINFS_NAME_ALIGN);

            //calculate dentry record length
            RecLen  = MINFS_DENTRY_LEN + NameLen;
            RecLen  = MINFS_ALIGN(RecLen, MINFS_DENTRY_ALIGN);

            //update minfs image paraters
            pMFSPara->DEntryLen += RecLen;
            pMFSPara->DEnrtyNum += 1;

        }
        else if (S_ISREG(s.st_mode))
        {
            ret = MINFS_CalcMFSFile(path,
                                    filename->d_name,
                                    pMFSPara,
                                    pConfig);
            if (ret != EPDK_OK)
            {
                MSG("calculate minfs image paramters "\
                    "failed when file [%s]\n", path);
                ret = EPDK_FAIL;
                break;
            }
        }
    }
    closedir(dir);

    //check this directory is root or not
    if (pMFSPara->RootDEntryNum == 0 && pMFSPara->RootDEntryLen == 0)
    {
        //the first level directory,
        //set minfs image root parameters.
        pMFSPara->RootDEntryNum = pMFSPara->DEnrtyNum;
        pMFSPara->RootDEntryLen = pMFSPara->DEntryLen;
    }
    return ret;
}

__s32 MINFS_CalcMFSImagePara(const char *pDir, __minfs_para_t *pMFSPara, __minfs_config_t *pConfig)
{
    __u32 ret = EPDK_OK;

    ret = MINFS_CalcMFSDir(pDir, pMFSPara, pConfig);
    if (ret != EPDK_OK)
    {
        MSG("Calculate diretory [%s] MINFS parameter failed\n", pDir);
        return EPDK_FAIL;
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
            ret = MINFS_CalcMFSImagePara(path, pMFSPara, pConfig);
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
