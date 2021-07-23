/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 */

#ifndef _SUNXI_RTC_H
#define _SUNXI_RTC_H

#ifdef CFG_SUNXI_BOOTREASON
enum {
       SUNXI_RTC_BOOT_REASON_NONE = 0,
       SUNXI_RTC_BOOT_REASON_COLD_BOOT,
       SUNXI_RTC_BOOT_REASON_HOT_REBOOT,
       SUNXI_RTC_BOOT_REASON_PANIC_REBOOT,
       SUNXI_RTC_BOOT_REASON_MAX,
};

int rtc_bootreason_init(void);
int rtc_set_bootreason_flag(uint32_t flag);
int rtc_get_bootreason_flag(uint32_t* bootreason);
int is_code_boot(void);
#endif

void rtc_write_data(int index, u32 val);
u32  rtc_read_data(int index);
void rtc_set_fel_flag(void);
u32  rtc_probe_fel_flag(void);
void rtc_clear_fel_flag(void);
void rtc_set_hash_entry(uint32_t entry);


#endif /* _SUNXI_RTC_H */
