/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_config.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-24
* Descript: Mini rom image config handing exteral header file.
* Update  : date                auther      ver     notes
*           2011-3-24 22:33:08  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __MINFS_CONFIG_H__
#define __MINFS_CONFIG_H__

//the max number of uncompress ext
#define MINFS_MAX_EXT_NUM    64

typedef struct __FILE_PARA
{
    __bool CompressON;
} __file_para_t;

typedef struct __MINFS_CONFIG
{
    __u32   Size;
    __u32   CExtNum;
    char    CExtTable[MINFS_MAX_EXT_NUM][_MAX_EXT];
} __minfs_config_t;

__s32 MINFS_ParseFileConfigPara(__minfs_config_t *pConfig,
                                const char *pFullPath,
                                __file_para_t *pFilePara);
__s32 MINFS_ParseConfigFile(const char *pFile, __minfs_config_t *pConfig);

#endif  //__MINFS_CONFIG_H__
