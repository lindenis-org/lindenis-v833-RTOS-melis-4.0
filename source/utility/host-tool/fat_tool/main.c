#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "basetypes.h"
#include "build.h"

typedef char TCHAR;

void Usage(void)
{
    printf("\n");
    printf("Usage:\n");
    printf(" 	fsbuild.exe config.ini spiltfile [-zero]\n");
    printf("\n");
    printf(" 	version300\n");
    printf(" 	2017-12-07\n");
    printf("\n");
}

u8 *LoadSplitData(const char *split_file, u32 *buflen)
{
    FILE *fp = NULL;
    u8 *buffer = NULL;

    if (NULL == split_file || NULL == buflen)
    {
        return NULL;
    }

    fp = fopen(split_file, "rb");
    if (fp == NULL)
    {
        printf("cannot found the file : %s \n ", split_file);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    *buflen = ftell(fp);
    buffer = (u8 *)malloc(*buflen);
    if (buffer == NULL)
    {
        fclose(fp);
        printf("magic buff malloc is error \n");
        return NULL;
    }
    fseek(fp, 0, SEEK_SET);
    fread(buffer, *buflen, sizeof(char), fp);

    fclose(fp);
    fp = NULL;

    return buffer;
}

u32 UnLoadSplitData(u8 *buffer, u32 buflen)
{
    if (NULL != buffer)
    {
        memset(buffer, 0, buflen);
        free(buffer);
        buffer = NULL;
    }

    return OK;
}

u32 MakeFileSystem(const char *config_file, u8 *SplitBuf, u32 SplitLen, bool bCutSplit)
{
    u32 ret = OK;
    ret = BuildFileSystem(config_file, SplitBuf, SplitLen, bCutSplit);
    return ret;
}

int main(int argc, TCHAR *argv[], TCHAR *envp[])
{
    u32 ret = OK;
    u8 *SplitBuf    = NULL;
    u32 SplitLen    = 0;
    bool bCutSplit   = 0;    //0 不用擦除magic文件
    //1 切割后擦除magic文件
    switch (argc)
    {
        case 2:
        case 3:
            bCutSplit = 0;  //0 不用擦除magic文件 数据内容
            break;
        case 4:
            if (0 != strcmp(argv[3], "-zero"))
            {
                Usage();
                return 0;
            }
            bCutSplit = 1;  //1 切割后擦除magic文件  数据内容
            break;
        default:
            Usage();
            return 0;
    }

    if (argc == 3 || argc == 4)
    {
        SplitBuf = LoadSplitData(argv[2], &SplitLen);
        if (SplitBuf == NULL)
        {
            printf("GetMagicFileData failed !\n ");
            ret = __LINE__;
            goto Exit_End;
        }
    }

    ret = MakeFileSystem(argv[1], SplitBuf, SplitLen, bCutSplit);

Exit_End:
    if (NULL != SplitBuf)
    {
        UnLoadSplitData(SplitBuf, SplitLen);
        SplitBuf = NULL;
        SplitLen = 0;
    }

    if (OK != ret)
    {
        char str[128];
        sprintf(str, "fsbuild failed %d\n", ret);
        printf("%s\n", str);
    }

    return ret;
}
