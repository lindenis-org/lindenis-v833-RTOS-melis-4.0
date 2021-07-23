/*
 * =====================================================================================
 *
 *       Filename:  fix_file.c
 *
 *    Description:  fix the boot0 and boot1.
 *
 *        Version:  1.0
 *         Create:  2017-11-10 11:07:49
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  zhijinzeng@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2017-11-10 11:08:04
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#define MAX_PATH    512

int is_full_path(const char *filepath)
{
    if (filepath[0] && '/' == filepath[0])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void get_full_path(char *dname, const char *sname)
{
    char buffer[MAX_PATH];

    if (is_full_path(sname))
    {
        strcpy(dname, sname);
        return ;
    }

    /* Get the current working directory: */
    if (getcwd(buffer, MAX_PATH) == 0)
    {
        perror("getcwd error");
        return ;
    }

    sprintf(dname, "%s/%s", buffer, sname);
}


int main(int argc, char *argv[])
{
    FILE     *ifile;
    uint8_t  *buf;
    int32_t   length;
    char      fullpath[MAX_PATH];

    if (argc != 3)
    {
        printf("Usage:\n");
        printf("%s error\n", argv[0]);
        exit(-1);
    }

    memset(fullpath, 0x00, MAX_PATH);
    get_full_path(fullpath, argv[1]);

    if (access(fullpath, F_OK) != 0)
    {
        perror("File Not Exists!");
        exit(-1);
    }

    if (access(fullpath, R_OK) != 0)
    {
        perror("Cant Read Source File!\n");
        exit(-1);
    }

    ifile = fopen(fullpath, "rb+");
    if (ifile == NULL)
    {
        perror("open input file failure.");
        exit(-1);
    }

    int fix_length = 0;
    fix_length = atoi(argv[2]);
    fix_length = fix_length * 1024;

    fseek(ifile, 0, SEEK_END);
    length =  ftell(ifile);

    buf = malloc(fix_length);
    if (buf == NULL)
    {
        perror("Alloc file buffer failure!");
        exit(-1);
    }

    fseek(ifile, 0, SEEK_SET);
    if (fread(buf, length, 1, ifile) != 1)
    {
        perror("read file error!");
        exit(-1);
    }

    memset(buf + length, 0xff, fix_length - length);

    fseek(ifile, 0, SEEK_SET);
    fclose(ifile);

    ifile = fopen(fullpath, "wb+");
    if (ifile == NULL)
    {
        perror("open input file failure.");
        exit(-1);
    }

    if (fwrite(buf, fix_length, 1, ifile) != 1)
    {
        printf("write output file error!\n");
        exit(-1);
    }

    free(buf);
    fclose(ifile);

    printf("  ------%s fix file success!------\n", argv[1]);
    return 0;
}

