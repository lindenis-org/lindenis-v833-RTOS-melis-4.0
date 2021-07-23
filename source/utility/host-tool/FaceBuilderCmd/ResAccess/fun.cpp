#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PATH 260
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
    char *name_end = NULL;
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

    name_end = strrchr(fname, '.');

    memset(data, 0, strlen(data));
    strcpy(data, fname);
    memset(fname, 0, strlen(fname));

    delete_file_ext(data, fname);

    return fname;
}

int IsFileExist(char *pathname)
{
    if (pathname == NULL)
        return -1;
    if (access(pathname, F_OK) == 0)
        return 0;
    return -1;

}
unsigned int GetFileLength(signed char *filename)
{
    FILE *fp = NULL;
    fp = fopen((const char *)filename, "rb");
    if (fp == NULL)
    {
        return 0;
    }

    long position = 0;
    fseek(fp, 0, SEEK_END);
    unsigned int len = (unsigned int)ftell(fp) - position;
    fclose(fp);
    return len;
}
