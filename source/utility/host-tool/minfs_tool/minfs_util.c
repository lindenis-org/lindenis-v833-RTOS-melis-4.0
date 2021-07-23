/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_util.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-25
* Descript: Mini rom image file system.
* Update  : date                auther      ver     notes
*           2011-3-25 10:36:27  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "minfs_tool_i.h"

#include <stdio.h>
#include <string.h>

static void _split_whole_name(const char *whole_name, char *fname, char *ext)
{
    char *p_ext;

    p_ext = rindex(whole_name, '.');
    if (NULL != p_ext)
    {
        strcpy(ext, p_ext);
        snprintf(fname, p_ext - whole_name + 1, "%s", whole_name);
    }
    else
    {
        ext[0] = '\0';
        strcpy(fname, whole_name);
    }
}

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
{
    char *p_whole_name;

    drive[0] = '\0';
    if (NULL == path)
    {
        dir[0] = '\0';
        fname[0] = '\0';
        ext[0] = '\0';
        return;
    }

    if ('/' == path[strlen(path)])
    {
        strcpy(dir, path);
        fname[0] = '\0';
        ext[0] = '\0';
        return;
    }

    p_whole_name = rindex(path, '/');
    if (NULL != p_whole_name)
    {
        p_whole_name++;
        _split_whole_name(p_whole_name, fname, ext);

        snprintf(dir, p_whole_name - path, "%s", path);
    }
    else
    {
        _split_whole_name(path, fname, ext);
        dir[0] = '\0';
    }
}

#if 0
__bool IsDirectory(WIN32_FIND_DATA *pfinddata)
{
    if ((pfinddata->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
        return 1;
    }
    return 0;
}

__bool IsDot(WIN32_FIND_DATA *pfinddata)
{
    if (!IsDirectory(pfinddata))
    {
        return 0;
    }
    if (pfinddata->cFileName[0] == '.' &&
            pfinddata->cFileName[1] == '\0')
    {
        return 1;
    }
    return 0;
}

__bool IsDotDot(WIN32_FIND_DATA *pfinddata)
{
    if (!IsDirectory(pfinddata))
    {
        return 0;
    }
    if (pfinddata->cFileName[0] == '.' &&
            pfinddata->cFileName[1] == '.' &&
            pfinddata->cFileName[2] == '\0')
    {
        return 1;
    }
    return 0;
}


/*
*********************************************************************************************************
*                                     CHECK DIRECTORY VALID OR NOT
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__bool MINFS_ValidMinfsDir(WIN32_FIND_DATA *finddata)
{
    if (!IsDirectory(finddata))
    {
        return 0;
    }
    else if (IsDot(finddata))
    {
        return 0;
    }
    else if (IsDotDot(finddata))
    {
        return 0;
    }
    else if (stricmp(finddata->cFileName, ".svn") == 0)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*                                     CHECK FILE VALID OR NOT
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__bool MINFS_ValidMinfsFile(WIN32_FIND_DATA *finddata)
{
    if (IsDirectory(finddata))
    {
        return 0;
    }
    else if (stricmp(finddata->cFileName, "Thumbs.db") == 0)
    {
        return 0;
    }

    return 1;
}
#endif
