/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : boo1.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.05.21
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.05.21       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __boo1_h
#define  __boo1_h


#include "egon_def.h"


#define BOOT1_MAGIC                     "eGON.BT1"
#define MAGIC_FLAG                      "PHOENIX_CARD_IMG"


#define BOOT_SCRIPT_BUFFER_SIZE           (32 * 1024)
//定义存储类型
#define   START_FROM_NAND_FLASH       0
#define   START_FROM_SDCARD           1
#define   START_FROM_NOR_FLASH        2


//SD卡相关数据结构
typedef struct _boot_sdcard_info_t
{
    __s32               card_ctrl_num;                //总共的卡的个数
    __s32               boot_offset;                  //指定卡启动之后，逻辑和物理分区的管理
    __s32               card_no[4];                   //当前启动的卡号, 16-31:GPIO编号，0-15:实际卡控制器编号
    __s32               speed_mode[4];                //卡的速度模式，0：低速，其它：高速
    __s32               line_sel[4];                  //卡的线制，0: 1线，其它，4线
    __s32               line_count[4];                //卡使用线的个数
}
boot_sdcard_info_t;


typedef struct _boot_core_para_t
{
    __u32  user_set_clock;                 // 运行频率 M单位
    __u32  user_set_core_vol;              // 核心电压 mV单位
    __u32  vol_threshold;                  // 开机门限电压
}
boot_core_para_t;
/******************************************************************************/
/*                              file head of Boot1                            */
/******************************************************************************/

typedef struct __BOOT_DRAM_PARA
{
    __u32 base;
    __u32 size;
    __u32 clk;
    __u32 access_mode;
    __u32 cs_num;
    __u32 ddr8_remap;
    __u32 sdr_ddr;
    __u32 bwidth;
    __u32 col_width;
    __u32 row_width;
    __u32 bank_size;
    __u32 cas;
} boot_dram_para_t;


typedef struct _boot1_private_head_t
{
    __u32              prvt_head_size;
    __u8               prvt_head_vsn[4];                // the version of Boot1_private_hea
    __s32                       uart_port;              // UART控制器编号
    normal_gpio_cfg             uart_ctrl[2];           // UART控制器(调试打印口)GPIO信息
    boot_dram_para_t            dram_para;              // dram init para
    char                        script_buf[32 * 1024];  // 脚本参数
    boot_core_para_t            core_para;              // 关键参数
    __s32                       twi_port;               // TWI控制器编号
    normal_gpio_cfg             twi_ctrl[2];            // TWI控制器GPIO信息，用于控制TWI
    __s32                       debug_enable;           // debug使能参数
    __s32                       debug_log;           // debug使能参数
    __s32                       hold_key_min;           // hold key最小值
    __s32                       hold_key_max;           // hold key最大值
    __u32                       work_mode;              // 模式，区分量产还是升级
    __u32                       storage_type;           // 存储介质类型  0：nand   1：sdcard    2: spinor
    normal_gpio_cfg             storage_gpio[32];       // 存储设备 GPIO信息
    char                        storage_data[512 - sizeof(normal_gpio_cfg) * 32];      // 用户保留数据信息
    //boot_nand_connect_info_t    nand_connect_info;    // nand 参数
} boot1_private_head_t;

typedef struct _boot1_file_head_t
{
    boot_file_head_t      boot_head;
    boot1_private_head_t  prvt_head;
} boot1_file_head_t;


#endif     //  ifndef __boo1_h

/* end of boo1.h */
