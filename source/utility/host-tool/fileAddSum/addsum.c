#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basetypes.h"

#define BUF_LEN (32 * 1024)

#define SCOTT_DEBUG 1       // open debug function

u32 GetFileLength(s8 *filename)
{
    FILE *fp = NULL;
    fp = fopen((const char *)filename, "rb");
    if (fp == NULL)
    {
        return 0;
    }
    long position = ftell(fp);
    fseek(fp, 0, SEEK_END);
    u32 len = (u32)ftell(fp) - position;
    fclose(fp);
    return len;
}


u32 AddSum__old(u8 *buffer, u32 len, u32 org)
{
    u32 u32len = 0;
    u32 sum = org;
    u32 *pu32 = (u32 *) buffer;
    u32 last = 0;

    //首先处理4字节对齐的数据
    u32len = len / 4;
    u32 i;
    for (i = 0; i < u32len; i++)
    {
        sum += pu32[i];
        printf("sum=%X %X\n", sum, pu32[i]);
    }

    //如果有1 2 3字节的尾巴，则处理尾巴数据，按照lsb的格式
    if ((len % 4) != 0)
    {
        //len - u32len 范围是 1,2,3
        for (i = u32len; i < len; i++)
        {
            printf("\n%d\n", i);
            u32 byte = buffer[i];               //获取lsb的一个字节
            printf("byte=%x \n", byte);
            byte *= (1 << ((i - u32len) * 8));  //该字节进行加权
            printf("byte=%x \n", byte);
            last += byte;                       //累加到最后一个字节
            printf("last=%x \n", last);
        }
        sum += last;    //加上尾巴补全的u32数据
        printf("sum=%X last=%X\n", sum, last);
    }

    return sum;
}

//scott
//2009-12-08 16:16:58
u32 AddSum(u8 *buffer, u32 len, u32 org)
{
    u32 u32len  = 0;
    u32 sum     = org;
    u32 *pu32  = (u32 *) buffer;
    u32 last    = 0;
    u32 curlen  = 0;

    //首先处理4字节对齐的数据
    u32len = len >> 2;
    u32 i;
    for (i = 0; i < u32len; i++)
    {
        sum += pu32[i];
    }

    curlen = u32len << 2;
    //如果有1 2 3字节的尾巴，则处理尾巴数据，按照lsb的格式
    //if ((len % 4) != 0)
    if ((len & 0x3) != 0)
    {
        memcpy(&last, buffer + curlen, len - curlen);
        sum += last;    //加上尾巴补全的u32数据
    }

    return sum;
}

u32 WriteAddSumToFile(u32 addsum, char *addsumfile)
{
    FILE *fp = NULL;

    fp = fopen(addsumfile, "wb+");
    if (NULL == fp)
    {
        printf("Create %s failed !\n", addsumfile);
        return __LINE__;
    }

    fwrite(&addsum, 4, 1, fp);
    fclose(fp);
    fp = NULL;
    return 0;
}


int min(int arg1, int arg2)
{
    if (arg1 >= arg2)
        return arg2;
    else
        return arg1;
}

u32 CalcFileAddSum(char *infile, char *addsumfile)
{
    u32 ret = 0;
    FILE *fp = NULL;
    u8 buffer[1024];
    u32 FileLength = 0;
    u32 read_total = 0;
    u32 addsum = 0;

    FileLength = GetFileLength((s8 *)infile);
    if (0 == FileLength)
    {
        printf("File %s len == 0\n", infile);
        return __LINE__;
    }

    fp = fopen(infile, "rb");
    if (NULL == fp)
    {
        printf("open %s failed \n", infile);
        return __LINE__;
    }

    read_total = 0;
    while (read_total < FileLength)
    {
        u32 read = min(1024, (FileLength - read_total));
        memset(buffer, 0, 1024);

        if (1 != fread(buffer, read, 1, fp))
        {
            printf("read failed at %x\n", read_total);
            fclose(fp);
            fp = NULL;
            return __LINE__;
        }
        read_total += read;
        addsum = AddSum(buffer, read, addsum);
    }

    fclose(fp);
    fp = NULL;

    WriteAddSumToFile(addsum, addsumfile);

    return ret;
}


#define INVALID_HANDLE_VALUE NULL
typedef void *HANDLE;

u32 FileAddSum(char *infile, char *addsumfile)
{
    u32     ret         = 0;
    FILE   *fp          = NULL;
    u8     *buffer      = NULL;
    u32     buflen      = BUF_LEN;
    u32     FileLength  = 0;
    u32     read_total  = 0;
    u32     addsum      = 0;
    HANDLE  hFile       = INVALID_HANDLE_VALUE;
    u32     FileSizeHigh = 0;

    FileLength = GetFileLength((s8 *)infile);
    if (0 == FileLength)
    {
        printf("File %s len == 0\n", infile);
        ret = __LINE__;
        goto EXIT;
    }
    printf("FileLength=%x\n", FileLength);

    buffer = (u8 *) malloc(buflen);
    if (NULL == buffer)
    {
        printf("Malloc buffer failed !\n");
        ret = __LINE__;
        goto EXIT;
    }

    fp = fopen(infile, "rb");
    if (NULL == fp)
    {
        printf("open %s failed \n", infile);
        ret = __LINE__;
        goto EXIT;
    }

    printf("FileLength=%x FileSizeHigh=%X\n", FileLength, FileSizeHigh);

    read_total = 0;
    while (read_total < FileLength)
    {
        int dwRead = 0;
        u32 read = min(buflen, (FileLength - read_total));
        memset(buffer, 0, buflen);
        dwRead = fread(buffer, read, 1, fp);
        if (dwRead == 0)
        {
            printf("read failed at %x, error\n", read_total);
            fclose(fp);
            fp = NULL;
            ret = __LINE__;
            goto EXIT;
        }
        read_total += read * dwRead;
        addsum = AddSum(buffer, read * dwRead, addsum);
    }

    WriteAddSumToFile(addsum, addsumfile);

EXIT:

    if (NULL != fp)
    {
        fclose(fp);
        fp = NULL;
    }

    if (NULL != buffer)
    {
        free(buffer);
        buffer = NULL;
    }
    return ret;
}

void Usage(void)
{
    printf("\n");
    printf("Usage:\n");
    printf("FileaddSum infile addsumfile\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage();
        return -1;
    }

    FileAddSum(argv[1], argv[2]);
    return 0;
}
