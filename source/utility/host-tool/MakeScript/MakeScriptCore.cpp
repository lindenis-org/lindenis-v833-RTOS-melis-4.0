//------------------------------------------------------------------------------------------------------------
//
//  MakeScriptCore.cpp
//
//
//  2009-11-10 10:27:43
//
//  sunny
//
//------------------------------------------------------------------------------------------------------------

#ifndef  __MAKE_SCRIPT_CORE_CPP__
#define  __MAKE_SCRIPT_CORE_CPP__       1

#include <unistd.h>
#include <memory.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "error.h"
#include "basetypes.h"

#include "GenScript.h"
#include "SynScript.h"

#define   DUP_BUFFER_SIZE       2048
#define   SCRIPT_POSTFIX        "script"

char *find_file_name(char *name, int sep)
{
    char *name_start = NULL;
    if (NULL == name)
    {
        return NULL;
    }
    name_start = strrchr(name, sep);

    return (NULL == name_start) ? name : (name_start + 1);
}


static int DupFileContent(const char *SourceFile, const char *DestFile)
{
    u32   len   = 0;
    FILE *hSource   = NULL;
    FILE *hDest     = NULL;
    char DupBuffer[DUP_BUFFER_SIZE];

    hSource = fopen(SourceFile, "rb");
    if (NULL == hSource)
    {
        printf("open file %s fail\n", SourceFile);
        return __LINE__;
    }
    hDest = fopen(DestFile, "wb+");
    if (NULL == hDest)
    {
        fclose(hSource);
        printf("open file %s fail\n", DestFile);
        return __LINE__;
    }

    memset(DupBuffer, 0, DUP_BUFFER_SIZE);
    while ((len = fread(DupBuffer, 1, DUP_BUFFER_SIZE, hSource)) != 0)
    {
        if (fwrite(DupBuffer, 1, len, hDest) != len)
        {
            Err("DupFileContent", __FILE__, __LINE__, "write failed");
            return __LINE__;
        }
    }
    fclose(hSource);
    fclose(hDest);
    return OK;
}

static int GetLastDirName(char *fullpath, char *dirname)
{

    if ((NULL == fullpath) || (NULL == dirname))
    {
        printf("error:%d\n", __LINE__);
        return __LINE__;
    }

    //_splitpath(fullpath, drive, dir, fname, ext);
    strcpy(dirname, find_file_name(fullpath, '/'));
    return OK;
}

int MakeScriptFile(char *scandir, u32 BaseID, u32 MaxID)
{
    GenScriptCore GenEngine;
    SynScriptCore SynEngine;
    int  res;
    char DirName[MAX_PATH];
    char OldScriptName[MAX_PATH];
    char NewScriptName[MAX_PATH];

    memset(DirName,     0, MAX_PATH);
    memset(OldScriptName,   0, MAX_PATH);
    memset(NewScriptName,   0, MAX_PATH);

    GetLastDirName(scandir, DirName);
    sprintf(OldScriptName, "%s/%s.%s", scandir, DirName, SCRIPT_POSTFIX);
    sprintf(NewScriptName, "%s/%s_tmp.%s", scandir, DirName, SCRIPT_POSTFIX);

    // Generate engine work begin
    res = GenEngine.CreateScriptFile(NewScriptName);
    if (OK != res)
    {
        Err("MakeScriptFile", __FILE__, __LINE__, "%s", NewScriptName);
        Err("MakeScriptFile", __FILE__, __LINE__, "CreateScriptFile [%d]", res);
        return __LINE__;
    }
    res = GenEngine.GenScriptFile(scandir, BaseID, MaxID);
    if (OK != res)
    {
        Err("MakeScriptFile", __FILE__, __LINE__, "CreateScriptFile [%d]", res);
        return __LINE__;
    }
    GenEngine.CloseScriptFile();

    // if OldScriptName exist, Synchronization script files
    if (0 == access(OldScriptName, F_OK))
    {
        // Synchronization engine work begin
        res = SynEngine.LoadConfigPlug();
        if (OK != res)
        {
            Err("MakeScriptFile", __FILE__, __LINE__, "LoadConfigPlug [%d]", res);
            return __LINE__;
        }

        memset(DirName,         0, MAX_PATH);
        memset(OldScriptName,   0, MAX_PATH);
        memset(NewScriptName,   0, MAX_PATH);

        GetLastDirName(scandir, DirName);
        sprintf(OldScriptName, "%s/%s.%s", scandir, DirName, SCRIPT_POSTFIX);
        sprintf(NewScriptName, "%s/%s_tmp.%s", scandir, DirName, SCRIPT_POSTFIX);

        res = SynEngine.OpenScriptFiles(OldScriptName, NewScriptName);
        if (OK != res)
        {
            Err("MakeScriptFile", __FILE__, __LINE__, "OpenScriptFiles [%d]", res);
            return __LINE__;
        }
        res = SynEngine.SynScriptResItems();
        if (OK != res)
        {
            Err("MakeScriptFile", __FILE__, __LINE__, "SynScriptResItems [%d]", res);
            return __LINE__;
        }

        SynEngine.CloseScriptFiles();
    }

    // Dup new file content to old file content
    res = DupFileContent(NewScriptName, OldScriptName);
    if (OK != res)
    {
        Err("MakeScriptFile", __FILE__, __LINE__, "DupFileContent [%d]", res);
        return __LINE__;
    }

    remove(NewScriptName);
    return OK;
}

#endif      // __MAKE_SCRIPT_CORE_CPP__
