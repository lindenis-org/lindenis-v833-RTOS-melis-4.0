/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_main.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-25
* Descript: Mini rom image file system.
* Update  : date                auther      ver     notes
*           2011-3-25 13:39:59  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "config/config.h"
#include "minfs_tool_i.h"
#include <unistd.h>

static void PrintUserHelp(void)
{
    printf("	  MINFS文件系统PC工具使用说明           \n");
    printf("估算镜像空间                                \n");
    printf("minfs calc  source config                   \n");
    printf("source  用于估算镜像文件尺寸信息的源目录    \n");
    printf("config  镜像配置文件路径                    \n");
    printf("                                            \n");
    printf("创建镜像文件                                \n");
    printf("minfs make source dest config               \n");
    printf("source  创建镜像文件的源目录                \n");
    printf("dest    镜像文件文件路径                    \n");
    printf("config  镜像配置文件路径                    \n");
    printf("                                            \n");
    printf("导出镜像文件                                \n");
    printf("minfs dump souce dest                       \n");
    printf("source  镜像文件文件路径                    \n");
    printf("dest    镜像文件导出后存放的目标目录        \n");
    printf("By sunny at 2011-4-2 13:40:42.\n");

    printf("\n");
}

static int IsFullName(const char *FilePath)
{
    if ('/' == FilePath[0])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void GetFullPath(char *dName, const char *sName)
{
    char Buffer[MAX_PATH];

    if (IsFullName(sName))
    {
        strcpy(dName, sName);
        return ;
    }

    /* Get the current working directory: */
    if (getcwd(Buffer, MAX_PATH) == NULL)
    {
        perror("_getcwd error");
        return ;
    }
    sprintf(dName, "%s/%s", Buffer, sName);
}

int main(int argc, char *argv[])
{
    char    RootDir[MAX_PATH];
    char    DumpDir[MAX_PATH];
    char    FileName[MAX_PATH];
    char    ConfigFile[MAX_PATH];

    if (argc < 2)
    {
        PrintUserHelp();
        return -1;
    }

    if (strcasecmp(argv[1], "calc") == 0)
    {
        __u32               ImageLen;
        __minfs_para_t      MFSImagePara;
        __minfs_config_t    ImageConfig;

        //calcute minfs image information
        if (argc != 4)
        {
            //arguments error
            PrintUserHelp();
            return -1;
        }

        //convert relative path to full path
        GetFullPath(RootDir, argv[2]);
        GetFullPath(ConfigFile, argv[3]);

        //parser config file first
        memset(&ImageConfig, 0, sizeof(__minfs_config_t));
        if (MINFS_ParseConfigFile(ConfigFile, &ImageConfig) != EPDK_OK)
        {
            MSG("parser config file failed\n");
            return -1;
        }
        //initialize minfs image parameters
        memset(&MFSImagePara, 0, sizeof(__minfs_para_t));
        MFSImagePara.FDataCalcEn = 1;
        if (MINFS_CalcMFSImagePara(RootDir, \
                                   &MFSImagePara, &ImageConfig) != EPDK_OK)
        {
            MSG("calculate the minfs image information failed\n");
            return -1;
        }
        ImageLen = MINFS_HDR_LEN + \
                   MINFS_ALIGN(MFSImagePara.DEntryLen, MINFS_SECTOR_LEN) + \
                   MFSImagePara.FDataLen;
        //dump minfs image information
        MSG("----------------------------------------------------------------------\n");
        MSG("directory [%s] minfs image size information\n", RootDir);
        MSG("image file size    : [%8d]\n", ImageLen);
        MSG("DEntry count       : [%8d]\n", MFSImagePara.DEnrtyNum);
        MSG("DEntry length      : [%8d]\n", MFSImagePara.DEntryLen);
        MSG("Data   length      : [%8d]\n", MFSImagePara.FDataLen);
        MSG("----------------------------------------------------------------------\n");
    }
    else if (strcasecmp(argv[1], "make") == 0)
    {
        //create minfs image
        if (argc != 5)
        {
            //arguments error
            PrintUserHelp();
            return -1;
        }
        //convert relative path to full path
        //source dest config
        GetFullPath(RootDir, argv[2]);
        GetFullPath(FileName, argv[3]);
        GetFullPath(ConfigFile, argv[4]);
        if (MINFS_Create(RootDir, FileName, ConfigFile) != EPDK_OK)
        {
            MSG("make minfs image [%s] failed\n", FileName);
            return -1;
        }
    }
    else if (strcasecmp(argv[1], "dump") == 0)
    {
        //dump minfs image file
        if (argc != 4)
        {
            //arguments error
            PrintUserHelp();
            return -1;
        }
        GetFullPath(FileName, argv[2]);
        GetFullPath(DumpDir, argv[3]);
        MINFS_DumpImage(DumpDir, FileName);
    }
    else
    {
        //invalid command for minfs tool
        PrintUserHelp();
        return -1;
    }
    //succeeded return
    return 0;
}
