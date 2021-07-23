/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_config.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-24
* Descript: Mini rom image config handing functions.
* Update  : date                auther      ver     notes
*           2011-3-24 22:33:08  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include    "config/config.h"
#include    "minfs_tool_i.h"

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
__s32 MINFS_ParseFileConfigPara(__minfs_config_t *pConfig,
                                const char *pFullPath,
                                __file_para_t *pFilePara)
{
    __u32 Index;
    char  drive[_MAX_DRIVE];
    char  dir[_MAX_DIR];
    char  fname[_MAX_FNAME];
    char  ext[_MAX_EXT];

    //get filename extension
    _splitpath(pFullPath, drive, dir, fname, ext);

    for (Index = 0; Index < pConfig->CExtNum; Index++)
    {
        //ext include char '.', we should skip it
        if (strcasecmp(pConfig->CExtTable[Index], &ext[1]) == 0)
        {
            //file uncompress
            pFilePara->CompressON = 1;
            return EPDK_OK;
        }
    }

    //file data should compress
    pFilePara->CompressON = 0;
    return EPDK_OK;
}

__s32 MINFS_ParseConfigFile(const char *pFile, __minfs_config_t *pConfig)
{
    HCONFIG hCfg;
    __u32   ImageSize;
    __u32   ExtCount;
    __u32   CKeyNum;
    __u32   Index;
    char    ExtKey[MAX_KEY];

    if (pFile == NULL || pConfig == NULL)
    {
        MSG("invalid parameters for config parser\n");
        return EPDK_FAIL;
    }

    hCfg = OpenConfig(pFile, NULL);
    if (hCfg == NULL)
    {
        MSG("open config file [%s] failed\n", pFile);
        return EPDK_FAIL;
    }

    //get image size config
    if (GetKeyValue(hCfg, "IMAGE_CFG", "size", &ImageSize) != OK)
    {
        MSG("get image size form config file [%s] failed", pFile);
        ConfigDump(hCfg);
        return EPDK_FAIL;
    }
    pConfig->Size = ImageSize * 1024;

    //get uncompress section key number
    if (GetSectionKeyCount(hCfg, "COMPRESS_EXT", &CKeyNum) != OK)
    {
        MSG("get COMPRESS_EXT section number "\
            "form config file [%s] failed\n", pFile);
        ConfigDump(hCfg);
        return EPDK_FAIL;
    }

    //get uncompress ext number
    if (GetKeyValue(hCfg, "COMPRESS_EXT", "count", &ExtCount) != OK)
    {
        MSG("get uncompress ext number form config file [%s] failed\n", pFile);
        ConfigDump(hCfg);
        return EPDK_FAIL;
    }

    //check ext count valid or not
    if (ExtCount != (CKeyNum - 1))
    {
        MSG("config  file [%s] COMPRESS_EXT key count error\n", pFile);
        ConfigDump(hCfg);
        return EPDK_FAIL;
    }

    pConfig->CExtNum = ExtCount;

    //get uncompress exts
    for (Index = 0; Index < ExtCount; Index++)
    {
        sprintf(ExtKey, "%s%d", "compress", Index);
        if (GetKeyString(hCfg, "COMPRESS_EXT", \
                         ExtKey, pConfig->CExtTable[Index]) != OK)
        {
            MSG("get key [%s] string failed\n", ExtKey);
            ConfigDump(hCfg);
            return EPDK_FAIL;
        }
    }
    CloseConfig(hCfg);
    return EPDK_OK;
}
