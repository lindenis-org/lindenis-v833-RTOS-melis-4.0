/*
 * (C) Copyright 2018
 * wangwei <wangwei@allwinnertech.com>
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <private_boot0.h>
#include <private_uboot.h>
#include <private_toc.h>
#include <arch/clock.h>
#include <arch/uart.h>
#include <arch/dram.h>

typedef struct __fes_aide_info{
    __u32 dram_init_flag;       /* flag for Dram init */
    __u32 dram_update_flag;     /* flag for dram para */
    __u32 dram_paras[SUNXI_DRAM_PARA_MAX];
}fes_aide_info_t;

static void  note_dram_log(int dram_init_flag);
extern const boot0_file_head_t fes1_head;

int main(void)
{
	int dram_size=0;
	int status;

	sunxi_serial_init(fes1_head.prvt_head.uart_port, (void *)fes1_head.prvt_head.uart_ctrl, 2);
	printf("fes begin\n");
	status = sunxi_board_init();
	if(status)
		return 0;

	printf("beign to init dram\n");
#ifdef FPGA_PLATFORM
	dram_size = mctl_init((void *)fes1_head.prvt_head.dram_para);
#else
	dram_size = init_DRAM(0, (void *)fes1_head.prvt_head.dram_para);
#endif

	if (dram_size){
		note_dram_log(1);
		printf("init dram ok\n");
	} else {
		note_dram_log(0);
		printf("init dram fail\n");
	}

	mdelay(10);

	return dram_size;
}

static void  note_dram_log(int dram_init_flag)
{
    fes_aide_info_t *fes_aide = (fes_aide_info_t *)CONFIG_FES1_RET_ADDR;

    memset(fes_aide, 0, sizeof(fes_aide_info_t));
    fes_aide->dram_init_flag    = SYS_PARA_LOG;
    fes_aide->dram_update_flag  = dram_init_flag;

    memcpy(fes_aide->dram_paras, fes1_head.prvt_head.dram_para, SUNXI_DRAM_PARA_MAX * 4);
    memcpy((void *)DRAM_PARA_STORE_ADDR, fes1_head.prvt_head.dram_para, SUNXI_DRAM_PARA_MAX * 4);
}


