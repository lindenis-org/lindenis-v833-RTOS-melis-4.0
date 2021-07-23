//------------------------------------------------------------------------------------------------------------
//
//  GenScript.cpp
//
//  Scan document, generate script file
//
//  2009-11-9 17:49:44
//
//  sunny
//
//------------------------------------------------------------------------------------------------------------
#ifndef  __GEN_SCRIPT_CPP__
#define  __GEN_SCRIPT_CPP__ 1

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "basetypes.h"
#include "error.h"
#include "GenScript.h"
#include "script_type.h"

extern char *find_file_name(char *name, int sep);

static void StringToUpper(char *string)
{
    if (NULL == string)
    {
        return;
    }
    while (*string)
    {
        *string = toupper(*string);
        string++;
    }
}

int GenScriptCore::CreateScriptFile(const char *filename)
{
    if (NULL == filename)
    {
        return __LINE__;
    }
    if (NULL != ScriptFile)
    {
        Err("CreateScriptFile", __FILE__, __LINE__, "script open already");
        return __LINE__;
    }

    ScriptFile = fopen(filename, "w");
    if (NULL == ScriptFile)
    {
        Err("CreateScriptFile", __FILE__, __LINE__, "open file failed");
        return __LINE__;
    }
    return OK;
}

int GenScriptCore::CloseScriptFile(void)
{
    if (NULL == ScriptFile)
    {
        Err("CloseScriptFile", __FILE__, __LINE__, "script file closed already");
        return __LINE__;
    }
    fclose(ScriptFile);

    return OK;
}

int GenScriptCore::WriteOneResItem(FACE_RES_ITEM *ResItem)
{
    if (NULL == ResItem)
    {
        return __LINE__;
    }

    // Write ResItem
    fprintf(ScriptFile, "[%s]\n",               ResItem->Section);
    fprintf(ScriptFile, "name	  	=%s\n",    ResItem->Name);
    fprintf(ScriptFile, "file	  	=%s\n",    ResItem->File);
    fprintf(ScriptFile, "commentCN 	=%s\n",     ResItem->CommentCN);
    fprintf(ScriptFile, "commentEN 	=%s\n",     ResItem->CommentEN);
    fprintf(ScriptFile, "\n");

    return OK;
}

static int __GetLastDirName(char *fullpath, char *dirname)
{
    char *p;

    if ((NULL == fullpath) || (NULL == dirname))
    {
        return __LINE__;
    }

    p = (char *)(fullpath + strlen(fullpath));
    while ((*p == '\\' || *p == 0) && (p >= fullpath))
    {
        p--;
    }
    p++;
    *p = 0;         // cut string

    // break path name into components
    //_splitpath(fullpath, drive, dir, fname, ext);

    strcpy(dirname, find_file_name(fullpath, '/'));
    return OK;
}


char *get_file_ext(const char *path, char *ext)
{
    char *name_start = NULL;
    if (NULL == path)
    {
        return NULL;
    }
    char data[MAX_PATH];
    memset(data, 0, MAX_PATH);
    strcpy(data, path);
    char ch = '.';
    name_start = strrchr(data, ch);
    if (name_start == NULL)
        return NULL;
    strcpy(ext, name_start + 1);
    return ext;
}

char *get_file_dir(const char *path, char *dir)
{
    char *name_start = NULL;
    if (NULL == path)
    {
        return NULL;
    }
    char data[MAX_PATH];
    memset(data, 0, MAX_PATH);
    strcpy(data, path);

    char ch = '/';
    name_start = strrchr(data, ch);

    memcpy(dir, data, strlen(data) - strlen(name_start));
    return dir;
}

char *delete_file_ext(const char *path, char *dir)
{
    char *name_start = NULL;
    if (NULL == path)
    {
        return NULL;
    }
    char data[MAX_PATH];
    memset(data, 0, MAX_PATH);
    strcpy(data, path);

    char ch = '.';
    name_start = strrchr(data, ch);

    memcpy(dir, data, strlen(data) - strlen(name_start));
    return dir;
}

char *get_file_fname(const char *path, char *fname)
{
    char *name_start = NULL;
    if (NULL == path)
    {
        return NULL;
    }
    char data[MAX_PATH];
    memset(data, 0, MAX_PATH);
    strcpy(data, path);

    char ch = '/';
    name_start = strrchr(data, ch);

    strcpy(fname, name_start + 1);

    //name_end = strrchr(fname,'.');

    memset(data, 0, strlen(data));
    strcpy(data, fname);
    memset(fname, 0, strlen(fname));

    delete_file_ext(data, fname);

    return fname;
}

int GenScriptCore::GenOneResItem(const char *ResPath, u32 ResID)
{
    u32 res = OK;
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    char ResNameTmp[512];
    char LastDir[_MAX_DIR];
    FACE_RES_ITEM ResItem;

    memset(&ResItem, 0 , FACE_RES_ITEM_SIZE);
    memset(ResNameTmp, 0 , 512);

    // break path name into components
    //_splitpath(ResPath, drive, dir, fname, ext);
    memset(dir, 0, _MAX_DIR);
    memset(ext, 0, _MAX_EXT);
    memset(fname, 0, _MAX_FNAME);
    get_file_dir(ResPath, dir);
    get_file_ext(ResPath, ext);
    get_file_fname(ResPath, fname);

    // get last directory entry name
    __GetLastDirName(dir, LastDir);

    if ('.' == ext[0])
    {
        ext[0] = '_';       // change first '.' to '_'
    }
    StringToUpper(LastDir);
    StringToUpper(fname);
    StringToUpper(ext);
    sprintf(ResNameTmp, "%s_%s_%s", LastDir, fname, ext);
    if (strlen(ResNameTmp) > (RES_NAME_LEN - 1))
    {
        Err("GenOneResItem", __FILE__, __LINE__, "res name too long");
        return __LINE__;
    }

    // Set ResItem
    TRACE("%s\n", ResNameTmp);
    TRACE("%s\n", ResItem.Name);
    strcpy(ResItem.Name,        ResNameTmp);
    strcpy(ResItem.File,        ResPath);
    strcpy(ResItem.CommentCN,   fname);
    strcpy(ResItem.CommentEN,   fname);
    sprintf(ResItem.Section, "res_%u", ResID);

    res = WriteOneResItem(&ResItem);
    return res;
}

// Legal res file type
char *LegalResFileTable[] =
{
    "BMP",
    "JPG",
    NULL
};

int GenScriptCore::MatchResTypeFile(char *filename)
{
    char ext[_MAX_EXT];
    char **pResType;

    // break path name into components
    //_splitpath(filename, drive, dir, fname, ext);
    get_file_ext(filename, ext);

    // match res type file
    pResType = LegalResFileTable;
    while (*pResType)
    {
        if (0 == strcasecmp(*pResType, ext))
        {
            //Msg("Find [%s], match res type [%s]\n", filename, *pResType);
            return OK;
        }
        pResType++;
    }
    Msg("[%s] not match res type\n", filename);
    return __LINE__;
}

int GenScriptCore::IsResTypeFile(struct dirent *finddata)
{
    return MatchResTypeFile(finddata->d_name);
}

int GenScriptCore::GenScriptFile(const char *scandir, u32 BaseID, u32 MaxID)
{
    u32  res;
    u32  CurIDIndex;
    char finddir[MAX_PATH];
    char findmode[MAX_PATH];
    char fullpath[MAX_PATH];

    // 仅遍历当前目录下的所有资源文件
    memset(finddir, 0, MAX_PATH);
    strcpy(finddir, (char *)scandir);

    memset(findmode, 0, MAX_PATH);
    sprintf(findmode, "%s/*.*", scandir);//

    // 开始遍历
    DIR *d; //声明一个句柄
    struct dirent *file; //readdir函数的返回值就存放在这个结构体中
    char path [MAX_PATH];
    memset(path, 0, MAX_PATH);

    if (!(d = opendir(scandir)))
    {
        printf("error opendir %s!!!/n", path);
        return -1;
    }

    CurIDIndex = BaseID;
    while ((file = readdir(d)) != NULL)
    {
        if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
            continue;

        //非常好用的一个函数，比什么字符串拼接什么的来的快的多
        sprintf(path, "%s/%s", scandir, file->d_name);

        struct stat s;
        lstat(path, &s);

        if (S_ISREG(s.st_mode))
        {
            if (OK == IsResTypeFile(file))
            {
                // 处理资源文件
                memset(fullpath, 0 , MAX_PATH);
                sprintf(fullpath, "%s/%s", finddir,  file->d_name);
                res = GenOneResItem(fullpath, CurIDIndex);
                if (OK != res)
                {
                    Err("GenScriptFile", __FILE__, __LINE__, "FindFirstFile failed");
                    return __LINE__;
                }

                // 当前目录下的资源文件条目个数不可以超过
                // 最大资源ID号减去起始资源ID号
                if (CurIDIndex >= MaxID)
                {
                    Err("GenScriptFile", __FILE__, __LINE__, "ID overflow, res file too many");
                    return __LINE__;
                }
                CurIDIndex++;
            }

        }
    }
    closedir(d);

    Msg("Scan %d res items\n", CurIDIndex - BaseID);
    return OK;
}


GenScriptCore::GenScriptCore()
{
    ScriptFile = NULL;
}

GenScriptCore::~GenScriptCore()
{
    ScriptFile = NULL;
}

#endif      // __GEN_SCRIPT_CPP__
