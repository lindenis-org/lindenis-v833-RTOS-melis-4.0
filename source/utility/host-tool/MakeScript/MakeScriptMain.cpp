//------------------------------------------------------------------------------------------------------------
//
//  MakeScriptMain.cpp
//
//  MakeScript main entry
//
//  2009-11-10 11:42:56
//
//  sunny
//
//------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include "basetypes.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "MakeScriptCore.h"

static void PrintUserHelp(void)
{
    printf("\n");
    printf("MakeScript :\n");
    printf("MakeScript DirPath BaseID  MaxID\n");
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

    if (getcwd(Buffer, MAX_PATH) == NULL)
    {
        perror("_getcwd error");
        return ;
    }
    sprintf(dName, "%s/%s", Buffer, sName);
}

int main(int argc, char *argv[], char *envp[])
{
    int  res    = OK;
    u32  BaseID = 0;
    u32  MaxID  = 0;

    char szCurDir[MAX_PATH];
    if (argc != 4)
    {
        PrintUserHelp();
        return 1;
    }

    memset(szCurDir, 0, MAX_PATH);
    char *csDir = argv[1];

    if (!IsFullName(csDir))
    {
        getcwd(szCurDir, MAX_PATH);
        strcat(szCurDir, "/");
        GetFullPath(szCurDir, csDir);
    }

    BaseID = (u32)atoi((const char *)argv[2]);
    MaxID  = (u32)atoi((const char *)argv[3]);

    res = MakeScriptFile(szCurDir, BaseID, MaxID);
    if (OK != res)
    {
        printf("MakeScriptFile failed\n");
        return res;
    }
    return OK;
}
