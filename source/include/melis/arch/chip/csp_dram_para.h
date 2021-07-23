/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               DRAM CSP Module
*
*                                    (c) Copyright 2006-2010, Berg.Xing China
*                                             All Rights Reserved
*
* File    : csp_dram_para.h
* By      : Berg.Xing
* Version : v1.0
* Date    : 2010-12-2 13:24
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-2 13:24     Berg.Xing   1.0     build the file;
*           2011-1-26 14:00     cpl         1.1     modify for aw1619 system
*********************************************************************************************************
*/
#ifndef __CSP_DRAM_PARA_H__
#define __CSP_DRAM_PARA_H__
#include <typedef.h>

#define DRAM_PIN_DEV_ID     (0)
#define DRAM_PIN_LIST       ((__u32 *)0)
#define DRAM_PIN_NUMBER     (0)

typedef enum __DRAM_TYPE
{
    DRAM_TYPE_SDR   = 0,
    DRAM_TYPE_DDR   = 1,
    DRAM_TYPE_DDR2  = 2,
    DRAM_TYPE_MDDR = 3
} __dram_type_e;

typedef enum __DRAM_BNKMOD
{
    DRAM_ACS_INTERLEAVE = 0,
    DRAM_ACS_SEQUENCE   = 1,
} __dram_bnkmod_e;

typedef enum __DRAM_PRIO_LEVEL
{
    DRAM_PRIO_LEVEL_0 = 0,
    DRAM_PRIO_LEVEL_1,
    DRAM_PRIO_LEVEL_2,
    DRAM_PRIO_LEVEL_3,
    DRAM_PRIO_LEVEL_
} __dram_prio_level_e;

//Offset 0-4 is valid
typedef enum __BANDWIDTH_DEV
{
    BW_DEVICE_CPU = 0,
    BW_DEVICE_VE,
    BW_DEVICE_DISP,
    BW_DEVICE_DTMB,
    BW_DEVICE_OTHER,
    BW_DEVICE_TOTAL
} __bw_dev_e;

#if defined CONFIG_SOC_SUN3IW2 || defined CONFIG_SOC_SUN8I
typedef struct __DRAM_MASTER
{
    __u32 bandwidth_limit0;
    __u32 bandwidth_limit1;
    __u32 bandwidth_limit2;
    __u32 command_number;
    __u32 master_n_wait_time;
    __u32 master_n_qos_value;
    __u32 master_limit_enable;
} __dram_master_t;

typedef struct __DRAM_BANDW
{
    __u32 cpu_bw;
    __u32 dtmb_bw;
    __u32 ve_bw;
    __u32 disp_bw;
    __u32 other_bw;
    __u32 total_bw;
} __dram_bandw_t;

typedef struct __DRAM_BWCONF
{
    __u32 max_statistic;
    __u32 statistic_unit;
    __u32 bw_enable;
} __dram_bwconf_t;

//Offset 0-9 is valid
typedef enum __DRAM_DEV
{
    DRAM_DEVICE_CPU = 0,
    DRAM_DEVICE_DTMB,
    DRAM_DEVICE_MAHB,
    DRAM_DEVICE_DMA,
    DRAM_DEVICE_VE,
    DRAM_DEVICE_TS,
    DRAM_DEVICE_DI,
    DRAM_DEVICE_TVIN,
    DRAM_DEVICE_DE20,
    DRAM_DEVICE_ROT,
    DRAM_DEVICE_
} __dram_dev_e;

typedef struct __DRAM_PARA
{
    //normal configuration
    __u32        dram_clk;
    __u32        dram_type;      //dram_type         DDR2: 2             DDR3: 3     LPDDR2: 6   LPDDR3: 7   DDR3L: 31
    //__u32      lpddr2_type;  //LPDDR2 type       S4:0    S2:1    NVM:2
    __u32        dram_zq;        //do not need
    __u32        dram_odt_en;

    //control configuration
    __u32        dram_para1;
    __u32        dram_para2;

    //timing configuration
    __u32        dram_mr0;
    __u32        dram_mr1;
    __u32        dram_mr2;
    __u32        dram_mr3;
    __u32        dram_tpr0;  //DRAMTMG0
    __u32        dram_tpr1;  //DRAMTMG1
    __u32        dram_tpr2;  //DRAMTMG2
    __u32        dram_tpr3;  //DRAMTMG3
    __u32        dram_tpr4;  //DRAMTMG4
    __u32        dram_tpr5;  //DRAMTMG5
    __u32        dram_tpr6;  //DRAMTMG8

    //reserved for future use
    __u32        dram_tpr7;
    __u32        dram_tpr8;
    __u32        dram_tpr9;
    __u32        dram_tpr10;
    __u32        dram_tpr11;
    __u32        dram_tpr12;
    __u32        dram_tpr13;
} __dram_para_t;

#elif defined (CONFIG_SOC_SUN3IW1)
/* ??Ҫ??ȡdram???ݵ????????? */
typedef struct __DRAM_MASTER
{
    __u32 dram_prio_level;
    __u32 dram_threshold0;
    __u32 dram_threshold1;
    __u32 dram_request_num;
} __dram_master_t;

/* 要读取dram数据的设备名 */
typedef enum __DRAM_DEV
{
    DRAM_DEVICE_NULL = 0,
    DRAM_DEVICE_DAHB,
    DRAM_DEVICE_IAHB,
    DRAM_DEVICE_DDMA,
    DRAM_DEVICE_NDMA,
    DRAM_DEVICE_SD0,
    DRAM_DEVICE_SD1,
    DRAM_DEVICE_DEBE,
    DRAM_DEVICE_DEFE,
    DRAM_DEVICE_VE,
    DRAM_DEVICE_CSI,
    DRAM_DEVICE_DeInterlace,
    DRAM_DEVICE_TVD,
    DRAM_DEVICE_
} __dram_dev_e;
/* 要读取dram数据的设备名 */
typedef struct __DRAM_PARA
{
    __u32           base;           // dram base address
    __u32           size;           // dram size, based on     (unit: MByte)
    __u32           clk;            // dram work clock         (unit: MHz)
    __u32           access_mode;    // 0: interleave mode 1: sequence mode
    __u32           cs_num;         // dram chip count  1: one chip  2: two chip
    __u32           ddr8_remap;     // for 8bits data width DDR 0: normal  1: 8bits
    __dram_type_e   type;           // ddr/ddr2/sdr/mddr/lpddr/...
    __u32           bwidth;         // dram bus width
    __u32           col_width;      // column address width
    __u32           row_width;      // row address width
    __u32           bank_size;      // dram bank count
    __u32           cas;            // dram cas
} __dram_para_t;
#endif

#endif  //__CSP_DRAM_PARA_H__
