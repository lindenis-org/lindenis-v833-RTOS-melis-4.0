//------------------------------------------------------------------------------------------------------------
//
// FaceBuilderMain.cpp
//
// 2009-11-4 16:48:03
//
// sunny
//
//------------------------------------------------------------------------------------------------------------

//#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <basetypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "stdafx.h"
#include "FaceBuilderCore.h"
#include "BCString.h"

// The one and only application object
using namespace std;


extern BOOL IsRelatePath(LPCTSTR lpDir);
extern void GetParentDir(LPCTSTR lpFileName, LPTSTR lpDirRet);
extern void GetFullPath(LPCTSTR lpStandar, LPCTSTR lpRelatePath, LPTSTR lpRet);

void PrintUserHelp(void)
{
    printf("\n");
    printf("FaceBuilderCmd 工具使用说明:\n");
    printf("FaceBuilderCmd ConfigFile.ini\n");
    printf("\n");
}

int FaceBuilderIsFullName(const char *FilePath)
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

void FaceBuilderGetFullPath(char *dName, const char *sName)
{
    char Buffer[MAX_PATH];

    if (FaceBuilderIsFullName(sName))
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

int main(int argc, TCHAR *argv[], TCHAR *envp[])
{
    int res;
    char ConfigFile[MAX_PATH];
    FaceBuilderCore Builder;

    // initialize MFC and print and error on failure
    if (argc != 2)
    {
        PrintUserHelp();
        return 1;
    }

    FaceBuilderGetFullPath(ConfigFile, argv[1]);
    /*
        char szCurDir[MAX_PATH];
        strcpy(ConfigFile, argv[1]);
        if(!FaceBuilderIsFullName(argv[1]))
        {
            char szCurDir[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, szCurDir);
            getcwd(szCurDir, MAX_PATH);
            strcat(szCurDir, "/");
            GetFullPath(szCurDir, argv[1], ConfigFile);

        }
    */
    printf("configfile:%s\n", ConfigFile);

    //----------------------------------------------------------
    //  load config plug
    //----------------------------------------------------------
    res = Builder.LoadConfigPlug();
    if (res)
    {
        printf("加载config插件失败\n");
        return 1;
    }

    //----------------------------------------------------------
    //  load configfile
    //----------------------------------------------------------
    res = Builder.LoadConfigFile(ConfigFile);
    if (res)
    {
        printf("加载配置文件失败\n");
        return 1;
    }

    //----------------------------------------------------------
    //  build bin
    //----------------------------------------------------------
    res = Builder.Buildbin();
    if (res)
    {
        printf("BuildBin失败\n");
        return 1;
    }

    //----------------------------------------------------------
    //  build face
    //----------------------------------------------------------
    res = Builder.Buildface();
    if (res)
    {
        printf("BuildFace失败\n");
        return 1;
    }
    return res;
}
