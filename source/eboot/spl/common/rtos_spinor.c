/*
 * (C) Copyright 2019
 *    Allwinnertech
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 */
#include <common.h>
#include <spare_head.h>
#include <private_boot0.h>
#include <private_uboot.h>
#include <private_toc.h>
#include <arch/spinor.h>
#include <part_efi_nor.h>
#include <linux/ctype.h>
#include <arch/rtc.h>

extern u16 debug_mode;

#ifdef CFG_SPINOR_ENV
static u32 simple_atoi(char *s)
{
	int num = 0, flag = 0;
	int i;

	for(i = 0;i <= strlen((char *)s);i++) {
		if(s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] -'0';
		else if(s[0] == '-' && i == 0)
			flag =1;
		else
			break;
	}

	if(flag == 1)
		num = num * -1;

	return(num);
}
#endif

int load_freertos_from_spinor(void)
{
	sbrom_toc1_head_info_t	*toc1_head;
	u8  *tmp_buff = (u8 *)CONFIG_BOOTPKG_BASE;
	int start_sector = 0;
	uint start_sector_A = 0;
	uint start_sector_B = 0;
	uint sectors_A = 0;
	uint sectors_B = 0;
	uint total_size = 0;

	if (init_gpt_nor()) {
		printf("init spinor gpt fail\n");
		return -1;
	}

	get_part_info_by_name(CFG_SPINOR_FREERTOS_NAME_A, &start_sector_A, &sectors_A);
	get_part_info_by_name(CFG_SPINOR_FREERTOS_NAME_B, &start_sector_B, &sectors_B);
	if( start_sector_A == 0 && start_sector_B == 0 ) {
		printf("%s and %s part info all can't find\n",
				 CFG_SPINOR_FREERTOS_NAME_A, CFG_SPINOR_FREERTOS_NAME_B);
		return -1;
	}
#ifdef CFG_SPINOR_ENV
	char *rtosAB_now;
	char *rtosAB_next = "A";
	char *upgrade_available;
	int need_reboot = 0;
	int need_flush = 0;
	int need_increase_bootcount = 0;
	int bootcount;
	char bootcount_str[5];
	int bootlimit;

	fw_env_open();
	rtosAB_next = fw_getenv("rtosAB_next");
	rtosAB_now = fw_getenv("rtosAB_now");
	printf("rtosAB_next:%s\n", rtosAB_next);
	printf("rtosAB_now:%s\n", rtosAB_now);
	if (strcmp(rtosAB_next, rtosAB_now) != 0) {
		if (strncmp(rtosAB_next, "A", strlen("A")) == 0) {
			printf("update rtosAB_now to A\n");
			fw_env_write("rtosAB_now", "A");
		} else if (strncmp(rtosAB_next, "B", strlen("B")) == 0) {
			printf("update rtosAB_now to B\n");
			fw_env_write("rtosAB_now", "B");
		} else {
			printf("invalid rtosAB_next:%s\n", rtosAB_next);
		}
		need_flush = 1;
	}

	upgrade_available = fw_getenv("upgrade_available");
	if (strcmp(upgrade_available, "1") == 0)
		need_increase_bootcount = 1;
	else
		need_increase_bootcount = 0;

#ifdef CFG_SUNXI_BOOTREASON
	if (is_code_boot() == 1)
		need_increase_bootcount = 0;
#endif

	if (need_increase_bootcount) {
		bootcount = simple_atoi(fw_getenv("bootcount"));
		bootlimit = simple_atoi(fw_getenv("bootlimit"));
		bootcount += 1;
		printf("bootcount:%d bootlimit:%d\n", bootcount, bootlimit);
		sprintf(bootcount_str, "%d", bootcount);
		fw_env_write("bootcount", bootcount_str);
		if (bootcount > bootlimit) {
			printf("bootcount > bootlimit, switch system\n");
			fw_env_write("bootcount", "0");
			if (strncmp(rtosAB_next, "A", strlen("A")) == 0) {
				printf("switch to B\n");
				fw_env_write("rtosAB_next", "B");
				fw_env_write("rtosAB_now", "B");
			} else if (strncmp(rtosAB_next, "B", strlen("B")) == 0) {
				printf("switch to A\n");
				fw_env_write("rtosAB_next", "A");
				fw_env_write("rtosAB_now", "A");
			} else {
				printf("invalid rtosAB_next %s, switch to A\n", rtosAB_next);
				fw_env_write("rtosAB_next", "A");
				fw_env_write("rtosAB_now", "A");
			}
			need_reboot = 1;
		}
		need_flush = 1;
	}

	if (need_flush) {
		need_flush = 0;
		fw_env_flush();
	}

	rtosAB_next = fw_getenv("rtosAB_next");
	if (strncmp(rtosAB_next, "A", strlen("A")) == 0)
		start_sector = start_sector_A;
	else if (strncmp(rtosAB_next, "B", strlen("B")) == 0)
		start_sector = start_sector_B;
	else
		start_sector = start_sector_A;
	fw_env_close();

	if (need_reboot) {
		need_reboot = 0;
		printf("TODO:reboot here\n");
	}
#else
	char *rtosAB_next = "A";
	start_sector = start_sector_A;
#endif


	char uart_input_value = get_uart_input();

	if (uart_input_value == 'a') {
		debug_mode = 1;
		printf("detected user input a\n");
		start_sector = start_sector_A;
#ifdef CFG_SPINOR_ENV
		fw_env_open();
		fw_env_write("rtosAB_next", "A");
		fw_env_write("rtosAB_now", "A");
		fw_env_flush();
		fw_env_close();
#endif
	} else if (uart_input_value == 'b' ) {
		debug_mode = 1;
		printf("detected user input b\n");
		start_sector = start_sector_B;
#ifdef CFG_SPINOR_ENV
		fw_env_open();
		fw_env_write("rtosAB_next", "B");
		fw_env_write("rtosAB_now", "B");
		fw_env_flush();
		fw_env_close();
#endif
	}

	if(spinor_init(0)) {
		printf("spinor init fail\n");
		return -1;
	}

	printf("load freertos%s in %d\n", rtosAB_next, start_sector);

	if (spinor_read(start_sector, 1, (void *)tmp_buff)) {
		printf("the first data is error\n");
		return -1;
	}
	printf("Succeed in reading toc file head.\n");

	toc1_head = (struct sbrom_toc1_head_info *)tmp_buff;
	if (toc1_head->magic != TOC_MAIN_INFO_MAGIC) {
		printf("toc1 magic error\n");
		return -1;
	}
	total_size = toc1_head->valid_len;
	printf("The size of toc is %x.\n", total_size );

	if (spinor_read(start_sector, total_size/512, (void *)tmp_buff)) {
		printf("spinor read data error\n");
		return -1;
	}
#ifdef CFG_SPINOR_VERIFY_CHECKSUM
	if (verify_addsum(tmp_buff, total_size) == 0) {
		printf("Check is correct\n");
	} else {
		printf("Check is not correct\n");
		return -1;
	}
#endif
	printf("load toc done\n");

	return 0;
}
