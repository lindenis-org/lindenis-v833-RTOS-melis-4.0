/*
 * =====================================================================================
 *
 *       Filename:  checksum.c
 *
 *    Description:  sign boot1 and boot2.
 *
 *        Version:  2.0
 *         Create:  2017-11-10 11:07:49
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2017-11-10 11:08:04
 *
 * =====================================================================================
 */

#include "checksum.h"
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

int32_t gen_check_sum(uint8_t *mem_base, uint32_t size)
{
    uint32_t *buf;
    uint32_t count;
    uint32_t sum;
    boot_file_head_t  *bfh;

    bfh = (boot_file_head_t *)mem_base;

    /* 生成校验和 */
    assert(bfh->check_sum = STAMP_VALUE);      // 将STAMP_VALUE写入Boot_file_head中的“check_sum”字段

    bfh->check_sum = STAMP_VALUE;

    count = size >> 2;                         // 以 字（4bytes）为单位计数
    sum   = 0;
    buf   = (uint32_t *)mem_base;

    do
    {
        sum += *buf++;                         // 依次累加，求得校验和
        sum += *buf++;                         // 依次累加，求得校验和
        sum += *buf++;                         // 依次累加，求得校验和
        sum += *buf++;                         // 依次累加，求得校验和
    }
    while ((count -= 4) > (4 - 1));

    while (count-- > 0)
        sum += *buf++;

    bfh->check_sum = sum;                  // 恢复Boot_file_head中的“check_sum”字段的值

    return 0;
}


int32_t check_magic(uint8_t *mem_base, const char *magic)
{
    uint32_t i;
    uint32_t sz;
    unsigned char *p;
    boot_file_head_t *bfh;

    bfh = (boot_file_head_t *)mem_base;
    p = bfh->magic;
    for (i = 0, sz = sizeof(bfh->magic);  i < sz;  i ++)
    {
        if (*p++ != *magic++)
            return -1;
    }

    return 0;
}

uint32_t align_to_512(uint8_t **buf, uint32_t length)
{
    uint8_t *newbuf;
    uint32_t newlen;
    boot_file_head_t *bfh;

    newlen = ALIGN(length, 512);
    newbuf = malloc(newlen);

    if (newbuf == NULL)
    {
        printf("alig to 512: malloc failed.\n");
        exit(-1);
    }

    memset(newbuf, 0xff, newlen);
    memcpy(newbuf, *buf, length);

    free(*buf);
    *buf = newbuf;

    bfh = (boot_file_head_t *)newbuf;

    bfh->length = newlen;

    return newlen;
}

int main(int argc, char *argv[])
{
    FILE     *ifile;
    FILE     *ofile;
    uint8_t  *buf;
    int32_t   length, alignlen;
    char      fullpath[MAX_PATH];
    int32_t   ret;

    if (argc != 4)
    {
        printf("Usage:\n");
        printf("%s boottype input output.\n", argv[0]);
        exit(-1);
    }

    memset(fullpath, 0x00, MAX_PATH);
    get_full_path(fullpath, argv[2]);

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

    fseek(ifile, 0, SEEK_END);
    length =  ftell(ifile);
    if (length <= sizeof(boot_file_head_t))
    {
        perror("Wrong Length of This file");
        exit(-1);
    }

    buf = malloc(length);
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

    rewind(ifile);

    alignlen = align_to_512(&buf, length);

    if (atoi(argv[1]) == 0)
    {
        ret = check_magic(buf, BOOT0_MAGIC);
    }
    else if (atoi(argv[1]) == 1)
    {
        ret = check_magic(buf, BOOT1_MAGIC);
    }
    else
    {
        printf("boottype should either 0(boot0) or 1(boot1)\n");
        exit(-1);
    }

    if (ret != 0)
    {
        printf("magic corruption.!\n");
        exit(-1);
    }

    gen_check_sum(buf, alignlen);

    memset(fullpath, 0x00, MAX_PATH);
    get_full_path(fullpath, argv[3]);

    ofile = fopen(fullpath, "wb+");
    if (ofile == NULL)
    {
        perror("open output file failure.");
        exit(-1);
    }

    if (fwrite(buf, alignlen, 1, ofile) != 1)
    {
        printf("write output file error!\n");
        exit(-1);
    }

    free(buf);
    fclose(ofile);
    fclose(ifile);

    printf("  ------(%s)aligned to 512B and generate checksum success!------\n", argv[3]);
    return 0;
}

