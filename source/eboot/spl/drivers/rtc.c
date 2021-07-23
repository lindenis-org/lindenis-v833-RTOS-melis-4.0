
/*
 * SPDX-License-Identifier: GPL-2.0+
 * Sunxi RTC data area ops
 *
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 */

#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <arch/rtc.h>

#define CRASHDUMP_RESET_FLAG                (0x5AA55AA5)
#define CRASHDUMP_RESET_READY               (0x5AA55AA6)
#define CRASHDUMP_REFRESH_READY             (0x5AA55AA7)
#define EFEX_FLAG                           (0x5AA5A55A)
#define RTC_INDEX  2
#define RTC_BOOTREASON_INDEX 7
#define BOOTLOADER_FIX_BOOTREASON_FLAG (0x80000000)


void rtc_write_data(int index, u32 val)
{
	writel(val, SUNXI_RTC_DATA_BASE + index*4);
}

u32 rtc_read_data(int index)
{
	return readl(SUNXI_RTC_DATA_BASE + index*4);
}

void rtc_set_fel_flag(void)
{
	do {
		rtc_write_data(RTC_INDEX, EFEX_FLAG);
		asm volatile("DSB");
		asm volatile("ISB");
	} while (rtc_read_data(RTC_INDEX) != EFEX_FLAG);
}

u32 rtc_probe_fel_flag(void)
{
	u32 i , reg_value;

	for (i=0; i<=5; i++) {
		reg_value = rtc_read_data(i);
		if (reg_value)
			printf("rtc[%d] value = 0x%x\n", i, reg_value);
	}

	reg_value = rtc_read_data(RTC_INDEX);
	if (reg_value == EFEX_FLAG) {
		printf("eraly jump fel\n");
		return 1;
	} else if (reg_value == CRASHDUMP_RESET_FLAG){
		rtc_write_data(RTC_INDEX, CRASHDUMP_RESET_READY);
		do {
			mdelay(150);
			reg_value = rtc_read_data(RTC_INDEX);
		}
		while (reg_value != CRASHDUMP_REFRESH_READY);
		printf("carshdump mode , jump fel\n");
		return 1;
	}
	return 0;
}

void rtc_clear_fel_flag(void)
{
	do {
		rtc_write_data(RTC_INDEX, 0);
		asm volatile("DSB");
		asm volatile("ISB");
	} while (rtc_read_data(RTC_INDEX) != 0);
}

void rtc_set_hash_entry(uint32_t entry)
{
	do {
		rtc_write_data(RTC_INDEX, entry);
		asm volatile("DSB");
		asm volatile("ISB");
	} while (rtc_read_data(RTC_INDEX) != entry);
}

#ifdef CFG_SUNXI_BOOTREASON
int rtc_set_bootreason_flag(uint32_t flag)
{
	do {
		rtc_write_data(RTC_BOOTREASON_INDEX, flag);
		asm volatile("DSB");
		asm volatile("ISB");
	} while (rtc_read_data(RTC_BOOTREASON_INDEX) != flag);

	return 0;
}

int rtc_get_bootreason_flag(uint32_t* bootreason)
{
	*bootreason = rtc_read_data(RTC_BOOTREASON_INDEX);

	return 0;
}

int is_code_boot(void)
{
	u32 bootreason = 0;

	rtc_get_bootreason_flag(&bootreason);

	if ((bootreason & 0xffff) == SUNXI_RTC_BOOT_REASON_COLD_BOOT)
		return 1;
	else
		return 0;
}

int rtc_bootreason_init(void)
{
	u32 bootreason = 0;

	rtc_get_bootreason_flag(&bootreason);

	/* bootloader did not fix bootreason  */
	if ((bootreason & 0xffff) == SUNXI_RTC_BOOT_REASON_NONE) {
		/* none --> cold */
		/* printf("fix to cold boot, save boot reason\n"); */
		rtc_set_bootreason_flag(SUNXI_RTC_BOOT_REASON_COLD_BOOT);
		rtc_get_bootreason_flag(&bootreason);
	} else if ((bootreason & 0xffff) == SUNXI_RTC_BOOT_REASON_COLD_BOOT) {
		/* cold --> hot */
		/* printf("fix to hot reboot, save boot reason\n"); */
		rtc_set_bootreason_flag(SUNXI_RTC_BOOT_REASON_HOT_REBOOT);
		rtc_get_bootreason_flag(&bootreason);
	} else if ((bootreason & 0xffff) >= SUNXI_RTC_BOOT_REASON_MAX) {
		/* error --> hot */
		printf("reason large than max, fix to hot reboot, save boot reason\n");
		rtc_set_bootreason_flag(SUNXI_RTC_BOOT_REASON_HOT_REBOOT);
		rtc_get_bootreason_flag(&bootreason);
	}

	rtc_set_bootreason_flag(bootreason | BOOTLOADER_FIX_BOOTREASON_FLAG);
	rtc_get_bootreason_flag(&bootreason);

	/* if ((bootreason & 0xffff) == SUNXI_RTC_BOOT_REASON_COLD_BOOT) */
		/* printf("boot0 bootreason: cold boot\n"); */
	/* else if ((bootreason & 0xffff) == SUNXI_RTC_BOOT_REASON_HOT_REBOOT) */
		/* printf("boot0 bootreason: hot reboot\n"); */
	/* else */
		/* printf("boot0 bootreason: %08x\n", bootreason); */

	return 0;
}
#endif
