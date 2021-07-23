/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 */

#include <common.h>
#include <spare_head.h>
#include <nand_boot0.h>
#include <private_toc.h>
#include <private_boot0.h>
#include <private_uboot.h>
#include <arch/uart.h>


/* check: 0-success  -1:fail */
int verify_addsum(void *mem_base, u32 size)
{
#ifdef CFG_USE_DCACHE
	dcache_enable();
#endif
	u32 *buf;
	u32 count;
	u32 src_sum;
	u32 sum;
	sbrom_toc1_head_info_t *bfh;

	bfh = (sbrom_toc1_head_info_t *)mem_base;
	/*generate checksum*/
	src_sum = bfh->add_sum;
	bfh->add_sum = STAMP_VALUE;
	count = size >> 2;
	sum = 0;
	buf = (u32 *)mem_base;
	do
	{
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	}while( ( count -= 4 ) > (4-1) );

	while( count-- > 0 )
		sum += *buf++;

	bfh->add_sum = src_sum;

//	printf("sum=%x\n", sum);
//	printf("src_sum=%x\n", src_sum);
#ifdef CFG_USE_DCACHE
	dcache_disable();
#endif
	if( sum == src_sum )
		return 0;
	else
		return -1;
}

u32 g_mod( u32 dividend, u32 divisor, u32 *quot_p)
{
	if (divisor == 0) {
		*quot_p = 0;
		return 0;
	}
	if (divisor == 1) {
		*quot_p = dividend;
		return 0;
	}

	for (*quot_p = 0; dividend >= divisor; ++(*quot_p))
		dividend -= divisor;
	return dividend;
}

u8  *get_page_buf( void )
{

	return (u8 *)(CONFIG_SYS_DRAM_BASE + 1024 * 1024);
}

char get_uart_input(void)
{

	char c = 0;

#ifdef CFG_FAST_GET_UART_INPUT
	if (sunxi_serial_tstc()) {
		c = sunxi_serial_getc();
		printf("key press : %c\n", c);
	}
#else
	u32 start = 0;

	start = get_sys_ticks();
	while (1) {
		if (sunxi_serial_tstc()) {
			c = sunxi_serial_getc();
			printf("key press : %c\n", c);
			break;
		}

		/* test time: 30 ms */
		if (get_sys_ticks() - start > 30)
			break;

		udelay(500);
	}
#endif
	return c;
}

static uint8_t uboot_func_mask;
void set_uboot_func_mask(uint8_t mask)
{
	uboot_func_mask |= mask;
}

uint8_t get_uboot_func_mask(uint8_t mask)
{
	return uboot_func_mask & mask;
}
