/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               DRAM CSP Module
*
*                                    (c) Copyright 2006-2010, Berg.Xing China
*                                             All Rights Reserved
*
* File    : csp_dram_ops.h
* By      : Berg.Xing
* Version : v1.0
* Date    : 2010-12-2 13:24
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-2 13:24     Berg.Xing   1.0     build the file;
*           2011-1-26 14:00     cpl         1.1     modify for aw1619 system
*********************************************************************************************************
*/
#ifndef __CSP_DRAM_OPS_H__
#define __CSP_DRAM_OPS_H__
#include <typedef.h>
#include "csp_dram_para.h"
#include <kconfig.h>
/*
*********************************************************************************************************
*                                   DRAM SET EMRS
*
* Description: set EMRS for DDR memory and Mobile memory;
*
* Arguments  : emrs_id  EMRS ID;
*              emrs_val EMRS register value;
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
uint32_t CSP_DRAMC_set_emrs(uint32_t emrs_id, uint32_t emrs_val);

void CSP_DRAMC_master_open(void);
void CSP_DRAMC_master_close(void);
void CSP_DRAM_store_register_data(void);
void CSP_DRAM_restore_register_data(void);
/*
*********************************************************************************************************
*                                   DRAM ENTER SELF REFRESH
*
* Description: dram enter self-refresh;
*
* Arguments  : none
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_enter_selfrefresh(void);

/*
*********************************************************************************************************
*                                   DRAM EXIT SELF REFRESH
*
* Description: dram exit self-refresh;
*
* Arguments  : none;
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_exit_selfrefresh(void);

/*
*********************************************************************************************************
*                                   DRAM POWER CONTROL
*
* Description: dram power down or not;
*
* Arguments  : down     if need set dram power down;
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_power_down(uint32_t down);

/*
*********************************************************************************************************
*                                   DRAM SET AUTO FRESH CYCLE
*
* Description: setup auto refresh interval value;
*
* Arguments  :  para DRAM config parameter
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_set_autofresh_cycle(uint32_t clk);

/*
*********************************************************************************************************
*                                   DRAM CLK OUTPUT CONTROL
*
* Description: dram output enable/disable;
*
* Arguments  : switch    1: enter power down mode
*                        0: exit power down mode
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
int32_t CSP_DRAMC_access_enable(void);


/*
**********************************************************************************************************************
*                                   DRAM ACCESS DISABLE
*
* Description: dram access disable;
*
* Arguments  : none
*
* Returns    : result, EBSP_TRUE/EBSP_FALSE;
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t CSP_DRAMC_access_disable(void);

/*
*********************************************************************************************************
*                                   DRAM SET ACCESS BLOCK SIZE
*
* Description: dram set hardware access block size;
*
* Arguments  : mod      hardware module id;
*              blksize  access block size;
*
* Returns    : result, EBSP_TRUE/EBSP_FALSE;
*
* Note       :
*********************************************************************************************************
*/

/*
**********************************************************************************************************************
*                                               DRAM_SetPrioMode
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t CSP_DRAMC_set_master_configuration(__dram_dev_e mod, __dram_master_t *master);

/*
**********************************************************************************************************************
*                                               DRAM_Setwaitnum
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t CSP_DRAMC_set_bandwidth_window_configuration(void);

/*
**********************************************************************************************************************
*                                               GET DRAM SIZE
*
* Description: Get DRAM Size in MB unit;
*
* Arguments  : None
*
* Returns    : 32/64/128
*
* Notes      :
*
**********************************************************************************************************************
*/
uint32_t CSP_DRAMC_get_dram_size(void);

/*
**********************************************************************************************************************
*                                               DRAM_check_ddr_readpipe
*
* Description: check the best dram readpipe value
*
* Arguments  : para: dram configure parameters
*
* Returns    : regurn EBSP_TRUE
*
* Notes      :
**********************************************************************************************************************
*/
uint32_t CSP_DRAMC_scan_readpipe(uint32_t clk);

/*
*********************************************************************************************************
*                                   DRAM SET PAD DRIVE
*
* Description: dram set pad drive;
*
* Arguments  : clk_freq
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_set_pad_drive(uint32_t clk_freq);
__s32 CSP_DRAMC_init(__dram_para_t *para, __s32 mode);
__s32 CSP_DRAMC_set_master_config(__dram_dev_e mod, __dram_master_t *master);

#ifdef CONFIG_SOC_SUN3IW2
__u32 CSP_DRAMC_get_all_bw_counter(__dram_bandw_t *bandw);
__s32 CSP_DRAMC_set_master_default(__dram_dev_e mod);
__s32 CSP_DRAMC_get_master_config(__dram_dev_e mod, __dram_master_t *master);
__s32 CSP_DRAMC_master_counter_general_config(__dram_bwconf_t *bwconf);
#endif

#endif  //__CSP_DRAM_OPS_H__
