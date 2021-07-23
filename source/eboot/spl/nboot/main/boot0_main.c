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
#include <arch/rtc.h>

static void update_uboot_info(u32 uboot_base, u32 optee_base, u32 monitor_base, u32 rtos_base, u32 dram_size,
		u16 pmu_type, u16 uart_input, u16 key_input, u16 debug_mode);
static int boot0_clear_env(void);

extern u16 debug_mode;

void main(void)
{
	int dram_size;
	int status;
	u32 uboot_base = 0, optee_base = 0, monitor_base = 0, rtos_base = 0;
	u16 pmu_type = 0, key_input = 0; /* TODO: set real value */

	sunxi_serial_init(BT0_head.prvt_head.uart_port, (void *)BT0_head.prvt_head.uart_ctrl, 6);

	sunxi_serial_reset(BT0_head.prvt_head.uart_port);
#ifndef CFG_NOT_ALWAYS_PRINT_HELLO
	printf("HELLO! BOOT0 is starting %s %s!\n", __DATE__, __TIME__);
	printf("BOOT0 commit : %s\n", BT0_head.hash);
#endif


#if defined(CFG_SUNXI_FES)
	/* fes dont have debug_mode, do nothing */
#elif defined(CFG_SUNXI_SBOOT)
	debug_mode = toc0_config->debug_mode;
#else
	debug_mode = BT0_head.prvt_head.debug_mode;
#endif

#ifdef CFG_NOT_ALWAYS_PRINT_HELLO
	printf("HELLO! BOOT0 is starting %s %s!\n", __DATE__, __TIME__);
	printf("BOOT0 commit : %s\n", BT0_head.hash);
#endif

	char uart_input_value = get_uart_input();

	if (uart_input_value == '2') {
		debug_mode = 1;
		printf("detected user input 2\n");
		goto _BOOT_ERROR;
	} else if (uart_input_value == 's' ) {
		debug_mode = 1;
		printf("detected user input s\n");
	}


	status = sunxi_board_init();
	if(status)
		goto _BOOT_ERROR;

	if (rtc_probe_fel_flag()) {
		rtc_clear_fel_flag();
		goto _BOOT_ERROR;
	}

#ifdef CFG_SUNXI_BOOTREASON
	rtc_bootreason_init();
#endif

#ifdef FPGA_PLATFORM
	dram_size = mctl_init((void *)BT0_head.prvt_head.dram_para);
#else
	dram_size = init_DRAM(0, (void *)BT0_head.prvt_head.dram_para);
#endif
	if(!dram_size)
		goto _BOOT_ERROR;
	else {
		printf("dram size =%d\n", dram_size);
	}
	mmu_enable(dram_size);
	malloc_init(CONFIG_HEAP_BASE, CONFIG_HEAP_SIZE);

	status = load_package();
	if(status == 0 ) {
#ifdef CFG_USE_DCACHE
		dcache_enable();
#endif

		load_image(&uboot_base, &optee_base, &monitor_base, &rtos_base);
	}
	else
		goto _BOOT_ERROR;

	update_uboot_info(uboot_base, optee_base, monitor_base, rtos_base, dram_size,
			pmu_type, uart_input_value, key_input, debug_mode);
	mmu_disable( );

	printf("Jump to secend Boot.\n");

	if(monitor_base)
		boot0_jmp_monitor(monitor_base);
	else if(optee_base)
		boot0_jmp_optee(optee_base, uboot_base);
	else if(rtos_base) {
		/* If no rtos head, do not set dram size */
		if (!memcmp(((struct spare_rtos_head_t *)rtos_base)->boot_head.magic, "rtos", 4))
			((struct spare_rtos_head_t *)rtos_base)->rtos_dram_size = (unsigned int)dram_size;
		debug_mode = 1;
		printf("jump to rtos\n");
		boot0_jmp(rtos_base);
	}
	else
		boot0_jmp(uboot_base);

	while(1);
_BOOT_ERROR:
	boot0_clear_env();
	boot0_jmp(FEL_BASE);

}

static void update_uboot_info(u32 uboot_base, u32 optee_base, u32 monitor_base, u32 rtos_base, u32 dram_size,
		u16 pmu_type, u16 uart_input, u16 key_input, u16 debug_mode)
{
	if(rtos_base) {
		uboot_head_t *header = (uboot_head_t *) rtos_base;
		if (memcmp((void *)header->boot_head.magic, RTOS_MELIS_MAGIC, 8) == 0) {
			printf("Rtos type: %s\n", "melis");
			printf("Update melis head\n");
			memcpy((void *)&header->boot_data.dram_para, &BT0_head.prvt_head.dram_para, 24 * sizeof(int));
		}
		return;
	}

	uboot_head_t  *header = (uboot_head_t *) uboot_base;
	struct sbrom_toc1_head_info *toc1_head = (struct sbrom_toc1_head_info *)CONFIG_BOOTPKG_BASE;

	header->boot_data.boot_package_size = toc1_head->valid_len;
	header->boot_data.dram_scan_size = dram_size;
	memcpy((void *)header->boot_data.dram_para, &BT0_head.prvt_head.dram_para, 32 * sizeof(int));

	if(monitor_base)
		header->boot_data.monitor_exist = 1;
	if(optee_base)
		header->boot_data.secureos_exist = 1;

	header->boot_data.func_mask = get_uboot_func_mask(UBOOT_FUNC_MASK_ALL);
	update_flash_para(uboot_base);

	header->boot_data.pmu_type = pmu_type;
	header->boot_data.uart_input = uart_input;
	header->boot_data.key_input = key_input;
	header->boot_data.debug_mode = debug_mode;
}


static int boot0_clear_env(void)
{
	sunxi_board_exit();
	sunxi_board_clock_reset();
	mmu_disable();
	mdelay(10);

	return 0;
}
