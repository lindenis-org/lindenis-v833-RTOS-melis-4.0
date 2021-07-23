// update_uboot.cpp : Defines the entry point for the console application.
//

#include <malloc.h>
#include <string.h>
#include "types.h"
#include "check.h"
#include "script.h"
#include "egon_def.h"
#include "boot1_v2.h"
#include <ctype.h>
#include <unistd.h>

#define  MAX_PATH             (260)


int  script_length;
int  align_size;

void *script_file_decode(char *script_name);
int merge_uboot(char *source_uboot_name, char *script_name);

int update_for_uboot(char *uboot_name, char *script_buf,  int script_length, int storage_type);

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

void GetFullPath(char *dName, const char *sName)
{
    char Buffer[MAX_PATH];

    if (IsFullName(sName))
    {
        strcpy(dName, sName);
        return ;
    }

    /* Get the current working directory: */
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
    char   source_uboot_name[MAX_PATH];
    char   script_file_name[MAX_PATH];
    int    storage_type = 0;
    char   *script_buf = NULL;

    if (argc == 4)
    {
        if ((argv[1] == NULL) || (argv[2] == NULL) || (argv[3] == NULL))
        {
            printf("update error: one of the input file names is empty\n");

            return __LINE__;
        }
        if ((!strcmp(argv[3], "sdmmc_card")) || (!strcmp(argv[3], "SDMMC_CARD")))
        {
            storage_type = 1;
        }
        else if ((!strcmp(argv[3], "spinor_flash")) || (!strcmp(argv[3], "SPINOR_FLASH")))
        {
            storage_type = 2;
        }
    }
    else
    {
        Usage();

        return __LINE__;
    }
    GetFullPath(source_uboot_name,  argv[1]);
    GetFullPath(script_file_name,   argv[2]);


    printf("\n");
    printf("uboot file Path=%s\n", source_uboot_name);
    printf("script file Path=%s\n", script_file_name);
    printf("\n");
    //初始化配置脚本
    script_buf = (char *)script_file_decode(script_file_name);
    if (!script_buf)
    {
        printf("update uboot error: unable to get script data\n");

        goto _err_out;

    }

    script_parser_init(script_buf);
    //读取原始uboot
    if (update_for_uboot(source_uboot_name, script_buf, script_length, storage_type))
    {
        printf("update uboot error: update error\n");

        goto _err_out;
    }
    /*
        if(merge_uboot(source_uboot_name, script_file_name))
        {
            printf("update uboot error: merge error\n");

            goto _err_out;
        }
    */
_err_out:
    if (script_buf)
    {
        free(script_buf);
    }

    return 0;
}

int update_sdcard_info(void *gpio_info, void *card_info_buf)
{
    int    i, value[8];
    script_gpio_set_t       gpio_set;
    normal_gpio_cfg        *storage_gpio;
    boot_sdcard_info_t     *card_info;

    storage_gpio = (normal_gpio_cfg *)gpio_info;
    card_info = (boot_sdcard_info_t *)card_info_buf;

    memset(gpio_info, 0, 32 * sizeof(normal_gpio_cfg));
    //填写SDCARD参数
    for (i = 0; i < 1; i++)
    {
        char  card_str[32];

        card_info->card_no[0] = -1;
        memset(card_str, 0, 32);
        //strcpy(card_str, "sdcard0_para");
        strcpy(card_str, "card_boot0_para");
        //card_str[9] = '4' + card_no;
        storage_gpio += i * 8;

        if (!script_parser_fetch(card_str, "card_ctrl", value))
        {
            card_info->card_no[i] = value[0];
        }
        else
        {
            card_info->card_no[i] = -1;
            continue;
        }
        if (!script_parser_fetch(card_str, "card_high_speed", value))
        {
            card_info->speed_mode[i] = value[0];
        }
        if (!script_parser_fetch(card_str, "card_line", value))
        {
            card_info->line_sel[i] = value[0];
        }
        //获取CLK
        memset(&gpio_set, 0, sizeof(script_gpio_set_t));
        if (!script_parser_fetch(card_str, "SDC_CLK", (int *)&gpio_set))
        {
            storage_gpio[0].port      = gpio_set.port;
            storage_gpio[0].port_num  = gpio_set.port_num;
            storage_gpio[0].mul_sel   = gpio_set.mul_sel;
            storage_gpio[0].pull      = gpio_set.pull;
            storage_gpio[0].drv_level = gpio_set.drv_level;
            storage_gpio[0].data      = gpio_set.data;
        }
        else if (!script_parser_fetch(card_str, "sdc_clk", (int *)&gpio_set))
        {
            storage_gpio[0].port      = gpio_set.port;
            storage_gpio[0].port_num  = gpio_set.port_num;
            storage_gpio[0].mul_sel   = gpio_set.mul_sel;
            storage_gpio[0].pull      = gpio_set.pull;
            storage_gpio[0].drv_level = gpio_set.drv_level;
            storage_gpio[0].data      = gpio_set.data;
        }
        else
        {
            printf("update error: unable to find SDC%d CLOCK PIN\n", i);

            return -1;
        }
        //获取CMD
        memset(&gpio_set, 0, sizeof(script_gpio_set_t));
        if (!script_parser_fetch(card_str, "SDC_CMD", (int *)&gpio_set))
        {
            storage_gpio[1].port      = gpio_set.port;
            storage_gpio[1].port_num  = gpio_set.port_num;
            storage_gpio[1].mul_sel   = gpio_set.mul_sel;
            storage_gpio[1].pull      = gpio_set.pull;
            storage_gpio[1].drv_level = gpio_set.drv_level;
            storage_gpio[1].data      = gpio_set.data;
        }
        else if (!script_parser_fetch(card_str, "sdc_cmd", (int *)&gpio_set))
        {
            storage_gpio[1].port      = gpio_set.port;
            storage_gpio[1].port_num  = gpio_set.port_num;
            storage_gpio[1].mul_sel   = gpio_set.mul_sel;
            storage_gpio[1].pull      = gpio_set.pull;
            storage_gpio[1].drv_level = gpio_set.drv_level;
            storage_gpio[1].data      = gpio_set.data;
        }
        else
        {
            printf("update error: unable to find SDC%d CLOCK CMD\n", i);

            return -1;
        }
        //获取DATA0
        memset(&gpio_set, 0, sizeof(script_gpio_set_t));
        if (!script_parser_fetch(card_str, "SDC_D0", (int *)&gpio_set))
        {
            storage_gpio[2].port      = gpio_set.port;
            storage_gpio[2].port_num  = gpio_set.port_num;
            storage_gpio[2].mul_sel   = gpio_set.mul_sel;
            storage_gpio[2].pull      = gpio_set.pull;
            storage_gpio[2].drv_level = gpio_set.drv_level;
            storage_gpio[2].data      = gpio_set.data;
        }
        else if (!script_parser_fetch(card_str, "sdc_d0", (int *)&gpio_set))
        {
            storage_gpio[2].port      = gpio_set.port;
            storage_gpio[2].port_num  = gpio_set.port_num;
            storage_gpio[2].mul_sel   = gpio_set.mul_sel;
            storage_gpio[2].pull      = gpio_set.pull;
            storage_gpio[2].drv_level = gpio_set.drv_level;
            storage_gpio[2].data      = gpio_set.data;
        }
        else
        {
            printf("update error: unable to find SDC%d CLOCK DATA0\n", i);

            return -1;
        }
        if (1 != card_info->line_sel[i])
        {
            //获取DATA1
            memset(&gpio_set, 0, sizeof(script_gpio_set_t));
            if (!script_parser_fetch(card_str, "SDC_D1", (int *)&gpio_set))
            {
                storage_gpio[3].port      = gpio_set.port;
                storage_gpio[3].port_num  = gpio_set.port_num;
                storage_gpio[3].mul_sel   = gpio_set.mul_sel;
                storage_gpio[3].pull      = gpio_set.pull;
                storage_gpio[3].drv_level = gpio_set.drv_level;
                storage_gpio[3].data      = gpio_set.data;
            }
            else if (!script_parser_fetch(card_str, "sdc_d1", (int *)&gpio_set))
            {
                storage_gpio[3].port      = gpio_set.port;
                storage_gpio[3].port_num  = gpio_set.port_num;
                storage_gpio[3].mul_sel   = gpio_set.mul_sel;
                storage_gpio[3].pull      = gpio_set.pull;
                storage_gpio[3].drv_level = gpio_set.drv_level;
                storage_gpio[3].data      = gpio_set.data;
            }
            else
            {
                printf("update error: unable to find SDC%d CLOCK DATA1\n", i);

                return -1;
            }
            //获取DATA2
            memset(&gpio_set, 0, sizeof(script_gpio_set_t));
            if (!script_parser_fetch(card_str, "SDC_D2", (int *)&gpio_set))
            {
                storage_gpio[4].port      = gpio_set.port;
                storage_gpio[4].port_num  = gpio_set.port_num;
                storage_gpio[4].mul_sel   = gpio_set.mul_sel;
                storage_gpio[4].pull      = gpio_set.pull;
                storage_gpio[4].drv_level = gpio_set.drv_level;
                storage_gpio[4].data      = gpio_set.data;
            }
            else if (!script_parser_fetch(card_str, "sdc_d2", (int *)&gpio_set))
            {
                storage_gpio[4].port      = gpio_set.port;
                storage_gpio[4].port_num  = gpio_set.port_num;
                storage_gpio[4].mul_sel   = gpio_set.mul_sel;
                storage_gpio[4].pull      = gpio_set.pull;
                storage_gpio[4].drv_level = gpio_set.drv_level;
                storage_gpio[4].data      = gpio_set.data;
            }
            else
            {
                printf("update error: unable to find SDC%d CLOCK DATA2\n", i);

                return -1;
            }
            //获取DATA3
            memset(&gpio_set, 0, sizeof(script_gpio_set_t));
            if (!script_parser_fetch(card_str, "SDC_D3", (int *)&gpio_set))
            {
                storage_gpio[5].port      = gpio_set.port;
                storage_gpio[5].port_num  = gpio_set.port_num;
                storage_gpio[5].mul_sel   = gpio_set.mul_sel;
                storage_gpio[5].pull      = gpio_set.pull;
                storage_gpio[5].drv_level = gpio_set.drv_level;
                storage_gpio[5].data      = gpio_set.data;
            }
            else if (!script_parser_fetch(card_str, "sdc_d3", (int *)&gpio_set))
            {
                storage_gpio[5].port      = gpio_set.port;
                storage_gpio[5].port_num  = gpio_set.port_num;
                storage_gpio[5].mul_sel   = gpio_set.mul_sel;
                storage_gpio[5].pull      = gpio_set.pull;
                storage_gpio[5].drv_level = gpio_set.drv_level;
                storage_gpio[5].data      = gpio_set.data;
            }
            else
            {
                printf("update error: unable to find SDC%d CLOCK DATA3\n", i);

                return -1;
            }
        }
    }

    return 0;
}

int update_for_uboot(char *uboot_name, char *script_buf, int script_length, int storage_type)
{
    FILE *uboot_file = NULL;
    boot1_file_head_t  *boot1_head;
    char *uboot_buf = NULL;
    int   length = 0;
    int   i;
    int   ret = -1;
    int   value[8];
    script_gpio_set_t   gpio_set[32];

    uboot_file = fopen(uboot_name, "rb+");
    if (uboot_file == NULL)
    {
        printf("update uboot error : unable to open uboot file\n");
        goto _err_uboot_out;
    }
    fseek(uboot_file, 0, SEEK_END);
    length = ftell(uboot_file);
    fseek(uboot_file, 0, SEEK_SET);
    if (!length)
    {
        printf("update uboot error : uboot length is zero\n");
        goto _err_uboot_out;
    }
    uboot_buf = (char *)malloc(length);
    if (!uboot_buf)
    {
        printf("update uboot error : fail to malloc memory for uboot\n");
        goto _err_uboot_out;
    }
    fread(uboot_buf, length, 1, uboot_file);
    rewind(uboot_file);

    boot1_head = (boot1_file_head_t *)uboot_buf;
    //检查boot0的数据结构是否完整
    ret = check_file((unsigned int *)uboot_buf, boot1_head->boot_head.length, BOOT1_MAGIC);
    if (ret != CHECK_IS_CORRECT)
    {
        printf("update uboot error : uboot pre checksum error\n");
        goto _err_uboot_out;
    }
    //取出数据进行修正,UART参数
    if (!script_parser_fetch("uart_para", "uart_debug_port", value))
    {
        boot1_head->prvt_head.uart_port = value[0];
    }
    if (!script_parser_mainkey_get_gpio_cfg("uart_para", gpio_set, 32))
    {
        for (i = 0; i < 2; i++)
        {
            if (!gpio_set[i].port)
            {
                break;
            }
            boot1_head->prvt_head.uart_ctrl[i].port      = gpio_set[i].port;
            boot1_head->prvt_head.uart_ctrl[i].port_num  = gpio_set[i].port_num;
            boot1_head->prvt_head.uart_ctrl[i].mul_sel   = gpio_set[i].mul_sel;
            boot1_head->prvt_head.uart_ctrl[i].pull      = gpio_set[i].pull;
            boot1_head->prvt_head.uart_ctrl[i].drv_level = gpio_set[i].drv_level;
            boot1_head->prvt_head.uart_ctrl[i].data      = gpio_set[i].data;
        }
    }
    //取出数据进行修正,TWI参数
    if (!script_parser_fetch("twi_para", "twi_port", value))
    {
        boot1_head->prvt_head.twi_port = value[0];
    }
    if (!script_parser_mainkey_get_gpio_cfg("twi_para", gpio_set, 32))
    {
        for (i = 0; i < 2; i++)
        {
            if (!gpio_set[i].port)
            {
                break;
            }
            boot1_head->prvt_head.twi_ctrl[i].port      = gpio_set[i].port;
            boot1_head->prvt_head.twi_ctrl[i].port_num  = gpio_set[i].port_num;
            boot1_head->prvt_head.twi_ctrl[i].mul_sel   = gpio_set[i].mul_sel;
            boot1_head->prvt_head.twi_ctrl[i].pull      = gpio_set[i].pull;
            boot1_head->prvt_head.twi_ctrl[i].drv_level = gpio_set[i].drv_level;
            boot1_head->prvt_head.twi_ctrl[i].data      = gpio_set[i].data;
        }
    }
    //取出数据进行修正,电源相关参数
    if (!script_parser_fetch("boot_extend", "vol_threshold", value))
    {
        boot1_head->prvt_head.core_para.vol_threshold = value[0];
    }
    if (!script_parser_fetch("target", "core_vol", value))
    {
        boot1_head->prvt_head.core_para.user_set_core_vol = value[0];
    }


#if 0
    //取出数据进行修正,DRAM参数
    if (!script_parser_fetch("dram_para", "dram_baseaddr", value))
    {
        boot1_head->prvt_head.dram_para.dram_baseaddr = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_clk", value))
    {
        boot1_head->prvt_head.dram_para.dram_clk = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_type", value))
    {
        boot1_head->prvt_head.dram_para.dram_type = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_rank_num", value))
    {
        boot1_head->prvt_head.dram_para.dram_rank_num = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_chip_density", value))
    {
        boot1_head->prvt_head.dram_para.dram_chip_density = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_io_width", value))
    {
        boot1_head->prvt_head.dram_para.dram_io_width = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_bus_width", value))
    {
        boot1_head->prvt_head.dram_para.dram_bus_width = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_cas", value))
    {
        boot1_head->prvt_head.dram_para.dram_cas = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_zq", value))
    {
        boot1_head->prvt_head.dram_para.dram_zq = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_odt_en", value))
    {
        boot1_head->prvt_head.dram_para.dram_odt_en = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_size", value))
    {
        boot1_head->prvt_head.dram_para.dram_size = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_tpr0", value))
    {
        boot1_head->prvt_head.dram_para.dram_tpr0 = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_tpr1", value))
    {
        boot1_head->prvt_head.dram_para.dram_tpr1 = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_tpr2", value))
    {
        boot1_head->prvt_head.dram_para.dram_tpr2 = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_tpr3", value))
    {
        boot1_head->prvt_head.dram_para.dram_tpr3 = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_tpr4", value))
    {
        boot1_head->prvt_head.dram_para.dram_tpr4 = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_tpr5", value))
    {
        boot1_head->prvt_head.dram_para.dram_tpr5 = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_emr1", value))
    {
        boot1_head->prvt_head.dram_para.dram_emr1 = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_emr2", value))
    {
        boot1_head->prvt_head.dram_para.dram_emr2 = value[0];
    }
    if (!script_parser_fetch("dram_para", "dram_emr3", value))
    {
        boot1_head->prvt_head.dram_para.dram_emr3 = value[0];
    }
#endif
    //根据数据进行修正，修正target参数
    if (!script_parser_fetch("target", "boot_clock", value))
    {
        boot1_head->prvt_head.core_para.user_set_clock = value[0];
    }
    if (!script_parser_fetch("target", "dcdc2_vol", value))
    {
        boot1_head->prvt_head.core_para.user_set_core_vol = value[0];
    }
    if (!script_parser_fetch("target", "debugenable", value))
    {
        boot1_head->prvt_head.debug_enable = value[0];
    }
    if (!script_parser_fetch("target", "debug_log", value))
    {
        boot1_head->prvt_head.debug_log = value[0];
    }


    //修正存储设备信息
    if (storage_type == 2)
    {
        if (update_sdcard_info((void *)boot1_head->prvt_head.storage_gpio, (void *)boot1_head->prvt_head.storage_data))
        {
            goto _err_uboot_out;
        }
    }

    memcpy(boot1_head->prvt_head.script_buf, script_buf, script_length);
    memset(boot1_head->prvt_head.script_buf + script_length, 0xff, 32 * 1024 - script_length);

    //数据修正完毕
    //重新计算校验和
    gen_check_sum((void *)uboot_buf,  boot1_head->boot_head.length);
    //再检查一次
    ret = check_file((unsigned int *)uboot_buf, boot1_head->boot_head.length, BOOT1_MAGIC);
    if (ret != CHECK_IS_CORRECT)
    {
        printf("update uboot error : uboot after checksum error\n");
        goto _err_uboot_out;
    }
    fwrite(uboot_buf, length, 1, uboot_file);

_err_uboot_out:
    if (uboot_buf)
    {
        free(uboot_buf);
    }
    if (uboot_file)
    {
        fclose(uboot_file);
    }

    return ret;
}


void *script_file_decode(char *script_file_name)
{
    FILE  *script_file;
    void  *script_buf = NULL;
    //读取原始脚本
    script_file = fopen(script_file_name, "rb");
    if (!script_file)
    {
        printf("update error:unable to open script file\n");
        return NULL;
    }
    //获取原始脚本长度
    fseek(script_file, 0, SEEK_END);
    script_length = ftell(script_file);
    if (!script_length)
    {
        fclose(script_file);
        printf("the length of script is zero\n");

        return NULL;
    }
    //读取原始脚本
    script_buf = (char *)malloc(script_length);
    if (!script_buf)
    {
        fclose(script_file);
        printf("unable malloc memory for script\n");

        return NULL;;
    }
    fseek(script_file, 0, SEEK_SET);
    fread(script_buf, script_length, 1, script_file);
    fclose(script_file);

    return script_buf;
}


int reconstruct_uboot(char *buf, int length, char *script_buf, int script_length)
{
    boot_file_head_t  *head;
    int  total_length;
    int  script_align;
    char *tmp_start;

    head = (boot_file_head_t *)buf;

    script_align = script_length;
    if (script_length & (align_size - 1))
    {
        script_align = (script_length + align_size) & (~(align_size - 1));
    }
    total_length = script_align + length;
    head->length = total_length;
    head->boot1_length = length;
    tmp_start = buf + length;

    printf("boot1 length = %x\n", head->boot1_length);
    printf("total length = %x\n", head->length);
    memset(tmp_start, 0xff, script_align);
    memcpy(tmp_start, script_buf, script_length);
    if (gen_check_sum(buf, total_length))
    {
        return -1;
    }
    printf("checksum=%x\n", head->check_sum);

    return 0;
}


int merge_uboot(char *source_uboot_name, char *script_file_name)
{
    FILE   *src_file = NULL;
    FILE   *script_file;
    int    source_length, script_length;
    int    total_length;
    char   *pbuf_source, *pbuf_script;
    char   buffer[512];
    boot_file_head_t   *head;
    int    ret = -1;
    //读取原始boot1
    src_file = fopen(source_uboot_name, "rb+");
    if (src_file == NULL)
    {
        printf("update uboot error:unable to open uboot file\n");
        goto _err_merge_uboot_out;
    }
    //获取原始uboot长度
    fseek(src_file, 0, SEEK_SET);
    fread(buffer, 512, 1, src_file);
    head = (boot_file_head_t *)buffer;

    source_length = head->length;
    align_size    = head->align_size;
    fseek(src_file, 0, SEEK_SET);
    if (!source_length)
    {
        printf("update error:the length of boot1 is zero\n");
        goto _err_merge_uboot_out;
    }
    //读取原始脚本
    script_file = fopen(script_file_name, "rb");
    if (!script_file)
    {
        printf("update uboot error:unable to open script file\n");
        goto _err_merge_uboot_out;
    }
    //获取原始脚本长度
    fseek(script_file, 0, SEEK_END);
    script_length = ftell(script_file);
    if (!script_length)
    {
        printf("the length of script is zero\n");
        goto _err_merge_uboot_out;
    }
    //读取原始脚本
    pbuf_script = (char *)malloc(script_length);
    if (!pbuf_script)
    {
        printf("unable malloc memory for script\n");

        goto _err_merge_uboot_out;
    }
    fseek(script_file, 0, SEEK_SET);
    fread(pbuf_script, script_length, 1, script_file);
    fclose(script_file);
    script_file = NULL;
    //获取原始uboot内存
    total_length = source_length + script_length;
    if (total_length & (align_size - 1))
    {
        total_length = (total_length + align_size) & (~(align_size - 1));
    }

    pbuf_source = (char *)malloc(total_length);
    if (!pbuf_source)
    {
        goto _err_merge_uboot_out;
    }
    //读取boot1数据
    fread(pbuf_source, source_length, 1, src_file);
    fseek(src_file, 0, SEEK_SET);
    //处理数据
    reconstruct_uboot(pbuf_source, source_length, pbuf_script, script_length);
    //回写文件
    fwrite(pbuf_source, total_length, 1, src_file);
    //关闭文件
    fclose(src_file);
    src_file = NULL;
    ret = 0;
_err_merge_uboot_out:
    if (src_file)
    {
        fclose(src_file);

        src_file = NULL;
    }
    if (script_file)
    {
        fclose(script_file);

        script_file = NULL;
    }
    if (pbuf_source)
    {
        free(pbuf_source);

        pbuf_source = NULL;
    }
    if (pbuf_script)
    {
        free(pbuf_script);

        pbuf_script = NULL;
    }

    return ret;
}


