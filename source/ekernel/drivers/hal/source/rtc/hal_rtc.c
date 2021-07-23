/*
 * An RTC driver for Sunxi Platform of Allwinner SoC
 *
 * Copyright (c) 2013, Carlo Caione <carlo.caione@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */
#include <log.h>
#include <hal_thread.h>
#include <init.h>
#include <stdlib.h>
#include <interrupt.h>
#include <rtc/rtc.h>
#include <sunxi_hal_rtc.h>

//for debug
/* #define CONFIG_DRIVERS_RTC_DEBUG */
#ifdef CONFIG_DRIVERS_RTC_DEBUG
#define RTC_INFO(fmt, arg...) printf(fmt, ##arg)
#else
#define RTC_INFO(fmt, arg...) do {}while(0)
#endif

#define RTC_ERR(fmt, arg...) __err(fmt, ##arg)

static struct hal_rtc_dev sunxi_hal_rtc;

static struct hal_rtc_data_year data_year_param =
{
    .min        = 1970,
    .max        = 2097,
    .mask       = 0x7f,
    .yshift     = 16,
    .leap_shift = 23,
};

void hal_rtc_write_data(int index, u32 val)
{
    writel(val, SUNXI_RTC_DATA_BASE + index * 4);
}

u32 hal_rtc_read_data(int index)
{
    return readl(SUNXI_RTC_DATA_BASE + index * 4);
}

void rtc_set_fel_flag(void)
{
    do
    {
        hal_rtc_write_data(RTC_FEL_INDEX, EFEX_FLAG);
        asm volatile("DSB");
        asm volatile("ISB");
    } while (hal_rtc_read_data(RTC_FEL_INDEX) != EFEX_FLAG);
}

u32 rtc_probe_fel_flag(void)
{
    return hal_rtc_read_data(RTC_FEL_INDEX) == EFEX_FLAG ? 1 : 0;
}

void rtc_clear_fel_flag(void)
{
    do
    {
        hal_rtc_write_data(RTC_FEL_INDEX, 0);
        asm volatile("DSB");
        asm volatile("ISB");
    } while (hal_rtc_read_data(RTC_FEL_INDEX) != 0);
}

int rtc_set_bootmode_flag(u8 flag)
{
    do
    {
        hal_rtc_write_data(RTC_BOOT_INDEX, flag);
        asm volatile("DSB");
        asm volatile("ISB");
    } while (hal_rtc_read_data(RTC_BOOT_INDEX) != flag);

    return 0;
}

int rtc_get_bootmode_flag(void)
{
    uint boot_flag;

    boot_flag = hal_rtc_read_data(RTC_BOOT_INDEX);

    return boot_flag;
}


int hal_rtc_register_callback(rtc_callback_t user_callback)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;

    if (user_callback == NULL)
    {
        return -1;
    }

    rtc_dev->user_callback = user_callback;

    return 0;
}


static irqreturn_t hal_rtc_alarmirq(int irq, void *dev)
{
    struct hal_rtc_dev *rtc_dev = (struct hal_rtc_dev *)dev;
    u32 val;

    val = readl(rtc_dev->base + SUNXI_ALRM_IRQ_STA);

    if (val & SUNXI_ALRM_IRQ_STA_CNT_IRQ_PEND)
    {
        val |= SUNXI_ALRM_IRQ_STA_CNT_IRQ_PEND;
        writel(val, rtc_dev->base + SUNXI_ALRM_IRQ_STA);

        if (rtc_dev->user_callback)
        {
            rtc_dev->user_callback();
        }

        return 0;
    }

    return -1;
}

static void hal_rtc_setaie(int to, struct hal_rtc_dev *rtc_dev)
{
    u32 alrm_val = 0;
    u32 alrm_irq_val = 0;

    if (to)
    {
        alrm_val = readl(rtc_dev->base + SUNXI_ALRM_EN);
        alrm_val |= SUNXI_ALRM_EN_CNT_EN;

        alrm_irq_val = readl(rtc_dev->base + SUNXI_ALRM_IRQ_EN);
        alrm_irq_val |= SUNXI_ALRM_IRQ_EN_CNT_IRQ_EN;
    }
    else
    {
        writel(SUNXI_ALRM_IRQ_STA_CNT_IRQ_PEND,
               rtc_dev->base + SUNXI_ALRM_IRQ_STA);
    }

    writel(alrm_val, rtc_dev->base + SUNXI_ALRM_EN);
    writel(alrm_irq_val, rtc_dev->base + SUNXI_ALRM_IRQ_EN);
}

static int hal_rtc_wait(int offset, unsigned int mask, unsigned int ms_timeout)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;
    u32 reg;

    msleep(ms_timeout);

    reg = readl(rtc_dev->base + offset);
    reg &= mask;

    if (reg != mask)
    {
        return 0;
    }

    return -1;
}

#ifdef SUNXI_SIMPLIFIED_TIMER
static short month_days[2][13] =
{
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static int hal_rtc_day_to_ymd(struct rtc_time *rtc_tm, u32 min_year,
                              u32 udate)
{
    int year = 0, leap, i;
    int date = (int)udate;

    if (!date)
    {
        rtc_tm->tm_mday = 1;
        rtc_tm->tm_mon = 1;
        rtc_tm->tm_year = 70;
        return 0;
    }

    year = min_year;
    while (1)
    {
        if (is_leap_year(year))
        {
            if (date > 366)
            {
                year++;
                date -= 366;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (date > 365)
            {
                year++;
                date -= 365;
            }
            else
            {
                break;
            }
        }
    }
    rtc_tm->tm_year = year - 1900;

    leap = is_leap_year(rtc_tm->tm_year);
    for (i = 1; date > month_days[leap][i]; i++)
    {
        date -= month_days[leap][i];
    }
    rtc_tm->tm_mon = i;
    rtc_tm->tm_mday = date;

    return 0;
}
#endif

int hal_rtc_gettime(struct rtc_time *rtc_tm)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;
    u32 date, time;

    /*
     * read again in case it changes
     */
    do
    {
        date = readl(rtc_dev->base + SUNXI_RTC_YMD);
        time = readl(rtc_dev->base + SUNXI_RTC_HMS);
    } while ((date != readl(rtc_dev->base + SUNXI_RTC_YMD)) ||
             (time != readl(rtc_dev->base + SUNXI_RTC_HMS)));

    rtc_tm->tm_sec  = SUNXI_TIME_GET_SEC_VALUE(time);
    rtc_tm->tm_min  = SUNXI_TIME_GET_MIN_VALUE(time);
    rtc_tm->tm_hour = SUNXI_TIME_GET_HOUR_VALUE(time);

#ifndef SUNXI_SIMPLIFIED_TIMER
    rtc_tm->tm_mday = SUNXI_DATE_GET_DAY_VALUE(date);
    rtc_tm->tm_mon  = SUNXI_DATE_GET_MON_VALUE(date);
    rtc_tm->tm_year = SUNXI_DATE_GET_YEAR_VALUE(date, rtc_dev->data_year);

    /*
     * switch from (data_year->min)-relative offset to
     * a (1900)-relative one
     */
    rtc_tm->tm_year += SUNXI_YEAR_OFF(rtc_dev->data_year);
#else
    hal_rtc_day_to_ymd(rtc_tm, rtc_dev->data_year->min, date);
#endif
    rtc_tm->tm_mon  -= 1;

    RTC_INFO("Read hardware RTC time %04d-%02d-%02d %02d:%02d:%02d",
             rtc_tm->tm_year + 1900, rtc_tm->tm_mon + 1, rtc_tm->tm_mday,
             rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);

    return 0;
}

int hal_rtc_settime(struct rtc_time *rtc_tm)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;
    u32 date = 0;
    u32 time = 0;
    int year;
#ifdef SUNXI_SIMPLIFIED_TIMER
    int i, leap;
#endif
    /*
     * the input rtc_tm->tm_year is the offset relative to 1900. We use
     * the SUNXI_YEAR_OFF macro to rebase it with respect to the min year
     * allowed by the hardware
     */

    year = rtc_tm->tm_year + 1900;
    if (year < rtc_dev->data_year->min
        || year > rtc_dev->data_year->max)
    {
        RTC_ERR("rtc only supports year in range %d - %d",
                rtc_dev->data_year->min, rtc_dev->data_year->max);
        return -1;
    }

#ifndef SUNXI_SIMPLIFIED_TIMER
    rtc_tm->tm_year -= SUNXI_YEAR_OFF(rtc_dev->data_year);
    rtc_tm->tm_mon += 1;

    RTC_INFO("Will set hardware RTC time %04d-%02d-%02d %02d:%02d:%02d",
             rtc_tm->tm_year, rtc_tm->tm_mon, rtc_tm->tm_mday,
             rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);

    date = SUNXI_DATE_SET_DAY_VALUE(rtc_tm->tm_mday) |
           SUNXI_DATE_SET_MON_VALUE(rtc_tm->tm_mon)  |
           SUNXI_DATE_SET_YEAR_VALUE(rtc_tm->tm_year, rtc_dev->data_year);

    if (is_leap_year(year))
    {
        date |= SUNXI_LEAP_SET_VALUE(1, rtc_dev->data_year->leap_shift);
    }
#else
    date = rtc_tm->tm_mday;
    rtc_tm->tm_mon += 1;
    leap = is_leap_year(year);
    for (i = 1; i < rtc_tm->tm_mon; i++)
    {
        date += month_days[leap][i];
    }

    for (i = year - 1; i >= rtc_dev->data_year->min; i--)
    {
        if (is_leap_year(i))
        {
            date += 366;
        }
        else
        {
            date += 365;
        }
    }
#endif

    time = SUNXI_TIME_SET_SEC_VALUE(rtc_tm->tm_sec)  |
           SUNXI_TIME_SET_MIN_VALUE(rtc_tm->tm_min)  |
           SUNXI_TIME_SET_HOUR_VALUE(rtc_tm->tm_hour);


    /*
     * before we write the RTC HH-MM-SS register,we
     * should check the SUNXI_LOSC_CTRL_RTC_HMS_ACC bit
     */
    if (hal_rtc_wait(SUNXI_LOSC_CTRL,
                     SUNXI_LOSC_CTRL_RTC_HMS_ACC, 50))
    {
        RTC_ERR("Failed to set rtc time.");
        return -1;
    }

    writel(0, rtc_dev->base + SUNXI_RTC_HMS);
    /*
     * After writing the RTC HH-MM-SS register, the
     * SUNXI_LOSC_CTRL_RTC_HMS_ACC bit is set and it will not
     * be cleared until the real writing operation is finished
     */

    if (hal_rtc_wait(SUNXI_LOSC_CTRL, SUNXI_LOSC_CTRL_RTC_HMS_ACC, 50))
    {
        RTC_ERR("Failed to set rtc time.");
        return -1;
    }

    writel(time, rtc_dev->base + SUNXI_RTC_HMS);

    /*
     * After writing the RTC HH-MM-SS register, the
     * SUNXI_LOSC_CTRL_RTC_HMS_ACC bit is set and it will not
     * be cleared until the real writing operation is finished
     */

    if (hal_rtc_wait(SUNXI_LOSC_CTRL, SUNXI_LOSC_CTRL_RTC_HMS_ACC, 50))
    {
        RTC_ERR("Failed to set rtc time.");
        return -1;
    }

    /*
     * After writing the RTC YY-MM-DD register, the
     * SUNXI_LOSC_CTRL_RTC_YMD_ACC bit is set and it will not
     * be cleared until the real writing operation is finished
     */

    if (hal_rtc_wait(SUNXI_LOSC_CTRL, SUNXI_LOSC_CTRL_RTC_YMD_ACC, 50))
    {
        RTC_ERR("Failed to set rtc time.");
        return -1;
    }
    msleep(2);

    writel(date, rtc_dev->base + SUNXI_RTC_YMD);

    /*
     * After writing the RTC YY-MM-DD register, the
     * SUNXI_LOSC_CTRL_RTC_YMD_ACC bit is set and it will not
     * be cleared until the real writing operation is finished
     */

    if (hal_rtc_wait(SUNXI_LOSC_CTRL, SUNXI_LOSC_CTRL_RTC_YMD_ACC, 50))
    {
        RTC_ERR("Failed to set rtc time.");
        return -1;
    }
    msleep(2);

    return 0;
}

int hal_rtc_getalarm(struct rtc_wkalrm *wkalrm)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;
    struct rtc_time *alrm_tm = &wkalrm->time;
    u32 alrm_en;
    u32 alarm_cur = 0, alarm_cnt = 0;
    unsigned long alarm_seconds = 0;
    int ret;

#ifdef SUNXI_ALARM1_USED
    alrm = readl(rtc_dev->base + SUNXI_ALRM_DHMS);
    date = readl(rtc_dev->base + SUNXI_RTC_YMD);

    alrm_tm->tm_sec = SUNXI_ALRM_GET_SEC_VALUE(alrm);
    alrm_tm->tm_min = SUNXI_ALRM_GET_MIN_VALUE(alrm);
    alrm_tm->tm_hour = SUNXI_ALRM_GET_HOUR_VALUE(alrm);

    alrm_tm->tm_mday = SUNXI_DATE_GET_DAY_VALUE(date);
    alrm_tm->tm_mon = SUNXI_DATE_GET_MON_VALUE(date);
    alrm_tm->tm_year = SUNXI_DATE_GET_YEAR_VALUE(date, rtc_dev->data_year);

    alrm_tm->tm_mon -= 1;

    /*
     * switch from (data_year->min)-relative offset to
     * a (1900)-relative one
     */
    alrm_tm->tm_year += SUNXI_YEAR_OFF(rtc_dev->data_year);
#else
    alarm_cnt = readl(rtc_dev->base + SUNXI_ALRM_COUNTER);
    alarm_cur = readl(rtc_dev->base + SUNXI_ALRM_CURRENT);

    RTC_INFO("alarm_cnt: %d, alarm_cur: %d", alarm_cnt, alarm_cur);
    if (alarm_cur > alarm_cnt)
    {
        /* alarm is disabled. */
        wkalrm->enabled = 0;
        alrm_tm->tm_mon = -1;
        alrm_tm->tm_mday = -1;
        alrm_tm->tm_year = -1;
        alrm_tm->tm_hour = -1;
        alrm_tm->tm_min = -1;
        alrm_tm->tm_sec = -1;
        return 0;
    }

    ret = hal_rtc_gettime(alrm_tm);
    if (ret)
    {
        return -1;
    }

    rtc_tm_to_time(alrm_tm, &alarm_seconds);
    alarm_cnt = (alarm_cnt - alarm_cur);
    alarm_cur = 0;
    alarm_seconds += alarm_cnt;

    rtc_time_to_tm(alarm_seconds, alrm_tm);
    RTC_INFO("alarm_seconds: %ld", alarm_seconds);

#endif

    alrm_en = readl(rtc_dev->base + SUNXI_ALRM_IRQ_EN);
    if (alrm_en & SUNXI_ALRM_EN_CNT_EN)
    {
        wkalrm->enabled = 1;
    }

    return 0;
}


int hal_rtc_setalarm(struct rtc_wkalrm *wkalrm)
{

    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;
    struct rtc_time *alrm_tm = &wkalrm->time;
    struct rtc_time tm_now;
    u32 alrm;
    time64_t diff;
    unsigned long time_gap;
    unsigned long time_gap_day;
#if defined(SUNXI_ALARM1_USED)
    unsigned long time_gap_hour = 0;
    unsigned long time_gap_min = 0;
#endif
    int ret;
    ret = hal_rtc_gettime(&tm_now);
    if (ret < 0)
    {
        RTC_ERR("Error in getting time");
        return -1;
    }

    diff = rtc_tm_sub(alrm_tm, &tm_now);

    if (diff <= 0)
    {
        RTC_ERR("Date to set in the past");
        return -1;
    }

    if (diff > 255 * SEC_IN_DAY)
    {
        RTC_ERR("Day must be in the range 0 - 255");
        return -1;
    }

    time_gap = diff;
    time_gap_day = alrm_tm->tm_mday - tm_now.tm_mday;
#ifdef SUNXI_SIMPLIFIED_TIMER
    hal_rtc_setaie(0, rtc_dev);

    writel(0, rtc_dev->base + SUNXI_ALRM_DAY);

    writel(0, rtc_dev->base + SUNXI_ALRM_HMS);

    writel(time_gap_day + readl(rtc_dev->base + SUNXI_RTC_YMD),
           rtc_dev->base + SUNXI_ALRM_DAY);

    alrm = SUNXI_ALRM_SET_SEC_VALUE(alrm_tm->tm_sec) |
           SUNXI_ALRM_SET_MIN_VALUE(alrm_tm->tm_min) |
           SUNXI_ALRM_SET_HOUR_VALUE(alrm_tm->tm_hour);

    writel(alrm, rtc_dev->base + SUNXI_ALRM_HMS);

#else
#ifdef SUNXI_ALARM1_USED
    time_gap -= time_gap_day * SEC_IN_DAY;
    time_gap_hour = time_gap / SEC_IN_HOUR;
    time_gap -= time_gap_hour * SEC_IN_HOUR;
    time_gap_min = time_gap / SEC_IN_MIN;
    time_gap -= time_gap_min * SEC_IN_MIN;
#endif

    hal_rtc_setaie(0, rtc_dev);
#ifdef SUNXI_ALARM1_USED
    if (hal_rtc_wait(SUNXI_LOSC_CTRL,
                     SUNXI_LOSC_CTRL_RTC_ALARM_ACC, 50))
    {
        RTC_ERR("Failed to set rtc alarm1.");
        return -1;
    }
    msleep(2);

    writel(0, rtc_dev->base + SUNXI_ALRM_DHMS);
    if (hal_rtc_wait(SUNXI_LOSC_CTRL,
                     SUNXI_LOSC_CTRL_RTC_ALARM_ACC, 50))
    {
        RTC_ERR("Failed to set rtc alarm1.");
        return -1;
    }
    msleep(2);

    alrm = SUNXI_ALRM_SET_SEC_VALUE(time_gap) |
           SUNXI_ALRM_SET_MIN_VALUE(time_gap_min) |
           SUNXI_ALRM_SET_HOUR_VALUE(time_gap_hour) |
           SUNXI_ALRM_SET_DAY_VALUE(time_gap_day);
    writel(alrm, rtc_dev->base + SUNXI_ALRM_DHMS);
    if (hal_rtc_wait(SUNXI_LOSC_CTRL,
                     SUNXI_LOSC_CTRL_RTC_ALARM_ACC, 50))
    {
        RTC_ERR("Failed to set rtc alarm1.");
        return -1;
    }
    msleep(2);
#else
    writel(0, rtc_dev->base + SUNXI_ALRM_COUNTER);
    alrm = time_gap;

    RTC_INFO("set alarm seconds:%d enable:%d", alrm, wkalrm->enabled);
    writel(alrm, rtc_dev->base + SUNXI_ALRM_COUNTER);
#endif

#endif

    writel(0, rtc_dev->base + SUNXI_ALRM_IRQ_EN);
    writel(SUNXI_ALRM_IRQ_EN_CNT_IRQ_EN, rtc_dev->base + SUNXI_ALRM_IRQ_EN);

    hal_rtc_setaie(wkalrm->enabled, rtc_dev);

    return 0;
}

int hal_rtc_alarm_irq_enable(unsigned int enabled)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;

    hal_rtc_setaie(enabled, rtc_dev);

    return 0;
}

void hal_rtc_min_year_show(unsigned int *min)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;

    *min = rtc_dev->data_year->min;

    RTC_INFO("sunxi rtc max year:%d", *min);
}

void hal_rtc_max_year_show(unsigned int *max)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;

    *max = rtc_dev->data_year->max;

    RTC_INFO("sunxi rtc max year:%d", *max);
}

int hal_rtc_init(void)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;
    int ret;
    unsigned int tmp_data;

    rtc_dev->base = SUNXI_RTC_BASE;
    rtc_dev->data_year = (struct hal_rtc_data_year *) &data_year_param;

    writel(0, rtc_dev->base + SUNXI_ALRM_COUNTER);

    /* disable alarm, not generate irq pending */
    writel(0, rtc_dev->base + SUNXI_ALRM_EN);

    /* disable alarm week/cnt irq, unset to cpu */
    writel(0, rtc_dev->base + SUNXI_ALRM_IRQ_EN);

    /* clear alarm week/cnt irq pending */
    writel(SUNXI_ALRM_IRQ_STA_CNT_IRQ_PEND, rtc_dev->base +
           SUNXI_ALRM_IRQ_STA);

    /* clear alarm wakeup output */
    writel(SUNXI_ALRM_WAKEUP_OUTPUT_EN, rtc_dev->base +
           SUNXI_ALARM_CONFIG);

    /*
    * Step1: select RTC clock source
    */
    tmp_data = readl(rtc_dev->base + SUNXI_LOSC_CTRL);
    tmp_data &= (~REG_CLK32K_AUTO_SWT_EN);

    /* Disable auto switch function */
    tmp_data |= REG_CLK32K_AUTO_SWT_BYPASS;
    writel(tmp_data, rtc_dev->base + SUNXI_LOSC_CTRL);

    tmp_data = readl(rtc_dev->base + SUNXI_LOSC_CTRL);
    tmp_data |= (RTC_SOURCE_EXTERNAL | REG_LOSCCTRL_MAGIC);
    writel(tmp_data, rtc_dev->base + SUNXI_LOSC_CTRL);

    /* We need to set GSM after change clock source */
    tmp_data = readl(rtc_dev->base + SUNXI_LOSC_CTRL);
    tmp_data |= (EXT_LOSC_GSM | REG_LOSCCTRL_MAGIC);
    writel(tmp_data, rtc_dev->base + SUNXI_LOSC_CTRL);
    rtc_dev->irq = SUXNI_IRQ_RTC;

    ret = request_irq(rtc_dev->irq, hal_rtc_alarmirq, 0, "rtc-ctrl", rtc_dev);
    if (ret)
    {
        RTC_ERR("Could not request IRQ");
        return -1;
    }
    enable_irq(rtc_dev->irq);
    RTC_INFO("RTC enabled");

    return 0;
}

int hal_rtc_uninit(void)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;
    free_irq(rtc_dev->irq, rtc_dev);
    return 0;
}

int hal_rtc_control(hal_rtc_transfer_cmd_t cmd, void *args)
{
    struct hal_rtc_dev *rtc_dev = &sunxi_hal_rtc;
    struct rtc_time *hal_rtc_time;
    struct rtc_wkalrm *hal_rtc_wkalarm;
    rtc_callback_t user_callback;
    unsigned int *enabled;

    switch (cmd)
    {
        case RTC_GET_TIME:
            hal_rtc_time = (struct rtc_time *)args;
            return hal_rtc_gettime(hal_rtc_time);
        case RTC_SET_TIME:
            hal_rtc_time = (struct rtc_time *)args;
            return hal_rtc_settime(hal_rtc_time);
        case RTC_GET_ALARM:
            hal_rtc_wkalarm = (struct rtc_wkalrm *)args;
            return hal_rtc_getalarm(hal_rtc_wkalarm);
        case RTC_SET_ALARM:
            hal_rtc_wkalarm = (struct rtc_wkalrm *)args;
            return hal_rtc_setalarm(hal_rtc_wkalarm);
        case RTC_CALLBACK:
            user_callback = (rtc_callback_t)args;
            return hal_rtc_register_callback(user_callback);
        case RTC_IRQENABLE:
            enabled = (unsigned int *)args;
            return hal_rtc_alarm_irq_enable(*enabled);
        default:
            return -1;
    }
}

const sunxi_hal_driver_rtc_t sunxi_hal_rtc_driver =
{
    .initialize     = hal_rtc_init,
    .uninitialize   = hal_rtc_uninit,
    .control        = hal_rtc_control,
};
