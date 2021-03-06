// update.cpp : Defines the entry point for the console application.
#include <malloc.h>
#include <stdlib.h>
#include "types.h"
#include <string.h>
#include "script.h"
#include "crc.h"
#include "sunxi_mbr.h"
#include <ctype.h>
#include <unistd.h>


#define  MAX_PATH  260

int IsFullName(const char *FilePath);
__s32  update_mbr_crc(sunxi_mbr_t *mbr_info, __s32 mbr_count, FILE *mbr_file);
__s32 update_for_part_info(sunxi_mbr_t *mbr_info, sunxi_download_info *dl_map, __s32 mbr_count, __s32 mbr_total_size);
__s32 update_dl_info_crc(sunxi_download_info *dl_map, FILE *dl_file);

/*
****************************************************************************************************
*       get_file_name
*
*  Description:
*     获取文件绝对路径名
*
*  Parameters:
*     buffer:文件名
*      length:文件绝对路径名
*
*  Return value:
****************************************************************************************************
*/
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

/* 判断是否为绝对路径*/
int IsFullName(const char *FilePath)
{
    if (FilePath[0] == '/')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
****************************************************************************************************
*
*       GetFullPath
*
*  Description:
*    获取绝对路径
*
*  Parameters:
*     dName:绝对路径文件名
*      sName:
*  Return value:
*
*
****************************************************************************************************
*/
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

//用法
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
    char   dl_name[MAX_PATH];
    FILE  *mbr_file = NULL;
    FILE  *dl_file = NULL;
    char  *pbuf = NULL;
    FILE  *source_file = NULL;
    int    script_len;
    int    mbr_count = 4, mbr_size;
    int    ret;
    sunxi_mbr_t    mbr_info;
    sunxi_download_info   dl_info;

    memset(source, 0, MAX_PATH);
    memset(mbr_name, 0, MAX_PATH);
    memset(dl_name, 0, MAX_PATH);
#if 1
    if (argc == 2)
    {
        if (argv[1] == NULL)
        {
            printf("update mbr error: one of the input file names is empty\n");
            return __LINE__;
        }

        GetFullPath(source,     argv[1]);
        mbr_count = 4;
    }
    else if (argc == 3)
    {
        if (argv[1] == NULL)
        {
            printf("update mbr error: one of the input file names is empty\n");
            return __LINE__;
        }

        GetFullPath(source,   argv[1]);
        mbr_count = atoi(argv[2]);
    }
    else
    {
        printf("parameters is invalid\n");
        return __LINE__;
    }
#else
    mbr_count = 4;
    GetFullPath(source,   str1);
//  GetFullPath(mbr_name, str2);
#endif
    GetFullPath(mbr_name, SUNXI_MBR_NAME);
    GetFullPath(dl_name, DOWNLOAD_MAP_NAME);

    printf("\n");
    printf("partitation file Path=%s\n", source);
    printf("mbr_name file Path=%s\n", mbr_name);
    printf("download_name file Path=%s\n", dl_name);
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
    if (script_parser_fetch("mbr", "size", &mbr_size) || (!mbr_size))
    {
        mbr_size = 16384;
    }
    //printf("mbr size = %d\n", mbr_size);
    mbr_size = mbr_size * 1024 / 512;
    //调用 script_parser_fetch函数取得任意数据项
    ret = update_for_part_info(&mbr_info, &dl_info, mbr_count, mbr_size);
    //printf("update_for_part_info %d\n", ret);
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
            ret = update_mbr_crc(&mbr_info, mbr_count, mbr_file);
            if (ret >= 0)
                dl_file = fopen(dl_name, "wb");
            if (!dl_file)
            {
                ret = -1;
                printf("update mbr fail: unable to create download map %s\n", dl_name);
            }
            else
            {
                ret = update_dl_info_crc(&dl_info, dl_file);
            }
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
    if (dl_file)
    {
        fclose(dl_file);
    }
    return 0;
}


__s32 update_for_part_info(sunxi_mbr_t *mbr_info, sunxi_download_info *dl_map, __s32 mbr_count, __s32 mbr_size)
{
    int  value[8];
    char fullname[260];
    char filename[32];
    int  part_index;
    int  part_handle;
    int  down_index, down_flag;
    int  udisk_exist, is_udisk;
    __int64  start_sector;
    int  ret;

    part_index = 0;
    down_index = 0;
    //初始化MBR
    memset(mbr_info, 0, sizeof(sunxi_mbr_t));
    memset(dl_map, 0, sizeof(sunxi_download_info));
    //固定不变的信息
    mbr_info->version = 0x00000200;
    strcpy((char *)mbr_info->magic, SUNXI_MBR_MAGIC);
    printf("mbr magic %s\n", mbr_info->magic);
    mbr_info->copy    = mbr_count;
    if (!mbr_count)
    {
        mbr_info->copy = SUNXI_MBR_COPY_NUM;
    }
    dl_map->version = 0x00000200;
    strcpy((char *)dl_map->magic, SUNXI_MBR_MAGIC);
    //根据分区情况会有变化的信息
    udisk_exist = 0;
    while (1)
    {
        down_flag = 0;
        is_udisk  = 0;
        part_handle = script_parser_fetch_partition();
        if (part_handle <= 0)
        {
            break;
        }
        strcpy((char *)mbr_info->array[part_index].classname, "DISK");
        //获取分区 name
        memset(value, 0, 8 * sizeof(int));
        if (!script_parser_fetch_mainkey_sub("name", part_handle, value))
        {
            printf("disk name=%s\n", (char *)value);
            if (!strcmp((char *)value, "UDISK"))
            {
                udisk_exist = 1;
                is_udisk    = 1;
            }
            strcpy((char *)mbr_info->array[part_index].name, (char *)value);
            memset(fullname, 0, 260);
            ret = script_parser_fetch_mainkey_sub("downloadfile", part_handle, (int *)fullname);
            if (!ret)
            {
                down_flag = 1;
                memset(filename, '0', 16);
                memset(filename + 16, 0, 16);
                get_file_name(fullname, filename);
                strcpy((char *)dl_map->one_part_info[down_index].name, (char *)value);
                memcpy((char *)dl_map->one_part_info[down_index].dl_filename, filename, 16);
            }
        }
        else
        {
            printf("MBR Create FAIL: Unable to find partition name in partition index %d\n", part_index);

            return -1;
        }
        //获取分区 用户类型
        if (!script_parser_fetch_mainkey_sub("user_type", part_handle, value))
        {
            mbr_info->array[part_index].user_type = value[0];
        }
        //获取分区 读写属性
        if (!script_parser_fetch_mainkey_sub("ro", part_handle, value))
        {
            mbr_info->array[part_index].ro = value[0];
        }
        //获取分区 写保护属性
        if (!script_parser_fetch_mainkey_sub("keydata", part_handle, value))
        {
            mbr_info->array[part_index].keydata = value[0];
        }
        //获取分区 是否需要校验
        if (down_flag)
        {
            value[0] = 1;
            ret = script_parser_fetch_mainkey_sub("verify", part_handle, value);
            if ((!ret) && (!value[0]))
            {
                dl_map->one_part_info[down_index].verify = 0;
            }
            else
            {
                dl_map->one_part_info[down_index].verify = value[0];
                memset((char *)dl_map->one_part_info[down_index].vf_filename, '0', 16);
                dl_map->one_part_info[down_index].vf_filename[0] = 'V';
                memcpy((char *)dl_map->one_part_info[down_index].vf_filename + 1, filename, 15);
            }
        }
        //获取分区 加密属性
        if (down_flag)
        {
            if (!script_parser_fetch_mainkey_sub("encrypt", part_handle, value))
            {
                dl_map->one_part_info[down_index].encrypt = value[0];
            }
        }
        if (!is_udisk)
        {
            //获取分区 大小低位
            if (!script_parser_fetch_mainkey_sub("size", part_handle, value))
            {
                mbr_info->array[part_index].lenlo = value[0];
                if (down_flag)
                {
                    dl_map->one_part_info[down_index].lenlo = value[0];
                }
            }
            else
            {
                printf("MBR Create FAIL: Unable to find partition size in partition %d\n", part_index);

                return -1;
            }
        }
        //生成分区 起始地址
        if (!part_index)
        {
            mbr_info->array[part_index].addrhi = 0;
            mbr_info->array[part_index].addrlo = mbr_size;
            if (down_flag)
            {
                dl_map->one_part_info[down_index].addrhi = 0;
                dl_map->one_part_info[down_index].addrlo = mbr_size;
            }
        }
        else
        {
            start_sector = mbr_info->array[part_index - 1].addrlo;
            start_sector |= (__int64)mbr_info->array[part_index - 1].addrhi << 32;
            start_sector += mbr_info->array[part_index - 1].lenlo;

            mbr_info->array[part_index].addrlo = (__u32)(start_sector & 0xffffffff);
            mbr_info->array[part_index].addrhi = (__u32)((start_sector >> 32) & 0xffffffff);

            if (down_flag)
            {
                dl_map->one_part_info[down_index].addrlo = mbr_info->array[part_index].addrlo;
                dl_map->one_part_info[down_index].addrhi = mbr_info->array[part_index].addrhi;
            }
        }

        part_index ++;
        if (down_flag)
        {
            down_index ++;
        }
    }

    if (part_handle < 0)
    {
        return -1;
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
    dl_map->download_count = down_index;
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
