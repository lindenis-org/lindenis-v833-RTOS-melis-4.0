#include <malloc.h>
#include "types.h"
#include <string.h>
#include "script.h"
#include "crc.h"
#include "sunxi_mbr.h"
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

#define  MAX_PATH  260

int IsFullName(const char *FilePath);
__s32 update_mbr_crc(sunxi_mbr_t *mbr_info, __s32 mbr_count, FILE *mbr_file);
__s32 update_for_part_info(sunxi_mbr_t *mbr_info, __s32 part_num);
__s32 update_dl_info_crc(sunxi_download_info *dl_map, FILE *dl_file);

/*
************************************************************************************************************
*       myitoa
*    函数名称：将数字转成字符串
*    参数列表：
*          num:待转数字
*          dest:保存转换结果的指针
*    返回值  ：
*    说明    ：
************************************************************************************************************
*/
int myitoa(int num, char *dest)
{
    if (dest == NULL)
        return -1;

    char temp[24];
    temp[23] = '\0';
    char *p = &temp[22];
    while (num / 10 != 0)
    {
        *(p--) = num % 10 + 48;
        num = num / 10;
    }
    *p = num % 10 + 48;
    strcpy(dest, p);
    return 0;
}

int get_file_name(char *path, char *name)
{
    char buffer[MAX_PATH];
    int  i, length;
    char *pt;

    memset(buffer, 0, MAX_PATH);
    if (!IsFullName(path))
    {
        if (getcwd(buffer, MAX_PATH) == NULL)
        {
            perror("getcwd error");
            return -1;
        }
        sprintf(buffer, "%s/%s", buffer, path);
    }
    else
    {
        strcpy(buffer, path);
    }

    length = strlen(buffer);
    pt = buffer + length - 1;

    while ((*pt != '/') && (*pt != '\\'))
    {
        pt --;
    }
    i = 0;
    pt ++;
    while (*pt)
    {
        if (*pt == '.')
        {
            name[i++] = '_';
            pt ++;
        }
        else if ((*pt >= 'a') && (*pt <= 'z'))
        {
            name[i++] = *pt++ - ('a' - 'A');
        }
        else
        {
            name[i++] = *pt ++;
        }

        if (i >= 16)
        {
            break;
        }
    }
    return 0;
}

int IsFullName(const char *FilePath)
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

void GetFullPath(char *dName, const char *sName)
{
    char Buffer[MAX_PATH];

    if (IsFullName(sName))
    {
        strcpy(dName, sName);
        return ;
    }

    if (getcwd(Buffer, MAX_PATH) == NULL)
    {
        perror("getcwd error");
        return ;
    }
    sprintf(dName, "%s/%s", Buffer, sName);
}

void Usage(void)
{
    printf("\n");
    printf("Usage:\n");
    printf("update.exe script file path para file path\n\n");
}


int main(int argc, char *argv[])
{
    char   source[MAX_PATH];
    char   mbr_name[MAX_PATH];

    FILE  *mbr_file = NULL;
    char  *pbuf = NULL;
    FILE  *source_file = NULL;

    int    script_len;
    int    part_num = 0;
    int    ret;
    sunxi_mbr_t    mbr_info;

    memset(source, 0, MAX_PATH);
    memset(mbr_name, 0, MAX_PATH);

    if (argc == 3)
    {
        if (argv[1] == NULL)
        {
            printf("error: the input file names is empty\n");

            return __LINE__;
        }

        GetFullPath(source, argv[1]);
    }
    else
    {
        printf("parameters is invalid\n");

        return __LINE__;
    }
    GetFullPath(mbr_name, argv[2]);

    printf("\n");
    printf("partitation file Path=%s\n", source);
    printf("out file Path=%s\n", mbr_name);
    printf("\n");
    //把文件打成一个ini脚本
    source_file = fopen(source, "rb");
    if (!source_file)
    {
        printf("unable to open script file\n");

        goto _err_out;
    }
    //读出脚本的数据
    //首先获取脚本的长度
    fseek(source_file, 0, SEEK_END);
    script_len = ftell(source_file);
    fseek(source_file, 0, SEEK_SET);
    //读出脚本所有的内容
    pbuf = (char *)malloc(script_len);
    if (!pbuf)
    {
        printf("unable to malloc memory for script\n");

        goto _err_out;
    }
    memset(pbuf, 0, script_len);
    fread(pbuf, 1, script_len, source_file);
    fclose(source_file);
    source_file = NULL;

    //script使用初始化
    if (SCRIPT_PARSER_OK != script_parser_init(pbuf))
    {
        goto _err_out;
    }

    if (script_parser_fetch("part_num", "num", &part_num))
    {
        printf("get part_num error\n");
    }
    printf("part_num = %d\n", part_num);

    //调用 script_parser_fetch函数取得任意数据项
    ret = update_for_part_info(&mbr_info, part_num);
    printf("update_for_part_info %d\n", ret);
    if (!ret)
    {
        mbr_file = fopen(mbr_name, "wb");
        if (!mbr_file)
        {
            ret = -1;
            printf("update mbr fail: unable to create file %s\n", mbr_name);
        }
        else
        {
            ret = update_mbr_crc(&mbr_info, 1, mbr_file);
        }
    }
    // script使用完成，关闭退出
    script_parser_exit();
    //打印结果
    if (!ret)
    {
        printf("update mbr file ok\n");
    }
    else
    {
        printf("update mbr file fail\n");
    }

_err_out:
    if (pbuf)
    {
        free(pbuf);
    }
    if (source_file)
    {
        fclose(source_file);
    }
    if (mbr_file)
    {
        fclose(mbr_file);
    }

    return 0;
}


__s32 update_for_part_info(sunxi_mbr_t *mbr_info, __s32 part_num)
{
    int  value[8];
    int  part_index;
    int  udisk_exist;
    __int64  start_sector;

    part_index = 0;
    //初始化MBR
    memset(mbr_info, 0, sizeof(sunxi_mbr_t));
    //固定不变的信息
    mbr_info->version = 0x00000100;

    strcpy((char *)mbr_info->magic, SUNXI_MBR_MAGIC);
    printf("mbr magic %s\n", mbr_info->magic);
    mbr_info->copy    = 1;
    mbr_info->index    = 0;

    //根据分区情况会有变化的信息
    udisk_exist = 0;
    int i = 0;
    for (i = 0; i < part_num; i++)
    {
        char data[10];
        char subname[32] = "partition";
        //data = itoa(i+1,data,10);
        myitoa(i, data);
        strcat(subname, data);

        if (i == 0)
            strcpy((char *)mbr_info->array[part_index].classname, "DISK");
        //获取分区 name
        memset(value, 0, 8 * sizeof(int));
        if (!script_parser_fetch(subname, "name", value))
        {
            printf("disk name=%s\n", (char *)value);
            if (!strcmp((char *)value, "UDISK"))
            {
                udisk_exist = 1;
                strcpy((char *)mbr_info->array[part_index].classname, "DISK");
                strcpy((char *)mbr_info->array[part_index].name, "UDISK");


            }
            //if (i == 0)
            //strcpy((char *)mbr_info->array[part_index].name, "ROOTFS");
            strcpy((char *)mbr_info->array[part_index].classname, "DISK");
            strcpy((char *)mbr_info->array[part_index].name, value);

        }
        else
        {
            printf("MBR Create FAIL: Unable to find partition name in partition index %d\n", part_index);

            return -1;
        }
        if (!script_parser_fetch(subname, "size_hi", value))
        {
            mbr_info->array[part_index].lenhi = value[0];
        }
        if (!script_parser_fetch(subname, "size_lo", value))
        {
            mbr_info->array[part_index].lenlo = value[0] * 2;
        }
        //生成分区 起始地址
        if (!part_index)
        {
            mbr_info->array[part_index].addrhi = 0;
            mbr_info->array[part_index].addrlo = 0x02;
        }
        else
        {
            start_sector = mbr_info->array[part_index - 1].addrlo;
            start_sector |= (__int64)mbr_info->array[part_index - 1].addrhi << 32;
            start_sector += mbr_info->array[part_index - 1].lenlo;

            mbr_info->array[part_index].addrlo = (__u32)(start_sector & 0xffffffff);
            mbr_info->array[part_index].addrhi = (__u32)((start_sector >> 32) & 0xffffffff);

        }
        part_index ++;
    }

    if (!udisk_exist)
    {
        start_sector = mbr_info->array[part_index - 1].addrlo;
        start_sector |= (__int64)mbr_info->array[part_index - 1].addrhi << 32;
        start_sector += mbr_info->array[part_index - 1].lenlo;

        mbr_info->array[part_index].addrlo = (__u32)(start_sector & 0xffffffff);
        mbr_info->array[part_index].addrhi = (__u32)((start_sector >> 32) & 0xffffffff);

        strcpy((char *)mbr_info->array[part_index].classname, "DISK");
        strcpy((char *)mbr_info->array[part_index].name, "UDISK");
        mbr_info->PartCount = part_index + 1;
    }
    else
    {
        mbr_info->PartCount = part_index;
    }

    return 0;
}

__s32  update_mbr_crc(sunxi_mbr_t *mbr_info, __s32 mbr_count, FILE *mbr_file)
{
    int i;
    unsigned crc32_total;

    for (i = 0; i < mbr_count; i++)
    {
        mbr_info->index = i;
        crc32_total = calc_crc32((void *) & (mbr_info->version), (sizeof(sunxi_mbr_t) - 4));
        mbr_info->crc32 = crc32_total;
        printf("crc %d = %x\n", i, crc32_total);
        fwrite(mbr_info, sizeof(sunxi_mbr_t), 1, mbr_file);
    }

    return 0;
}

__s32 update_dl_info_crc(sunxi_download_info *dl_map, FILE *dl_file)
{
    __u32 crc32_total;

    crc32_total = calc_crc32((void *) & (dl_map->version), (sizeof(sunxi_download_info) - 4));
    dl_map->crc32 = crc32_total;
    fwrite(dl_map, sizeof(sunxi_download_info), 1, dl_file);

    return 0;
}
