# RTC接口说明

## 代码结构

RTC模块的代码结构如下所示

```
melis-v3.0/source/ekernel/drivers
|--drv/source/rtc
|----drv_rtc.c //rt_thread接口代码
|--hal/source/rtc
|----hal_rtc.c //hal层接口代码
|----rtc-lib.c //rtc核心代码
|--test
|----test_rtc.c //rtc测试代码
```


## rt_thread接口

在 rt_thread框架里面，一个比较完整的使用RTC模块的代码如下所示：

```
#include <stdio.h>
#include <rtthread.h>
#include <sunxi_drv_rtc.h>
#include <sunxi_hal_rtc.h>

struct rtc_time rtc_tm;
struct rtc_wkalrm wkalrm;

int callback(void)  //闹钟回调函数
{
    printf("alarm interrupt\n");
    return 0;
}

static int main(int argc, const char **argv)
{
    rt_device_t fd;
    unsigned int enable = 0;

    fd = rt_device_find("rtc");  //搜索rtc模块
    if (fd == RT_NULL)
    {
        return -1;
    }

    rt_device_open(fd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM); //打开rtc模块
    rt_device_control(fd, RTC_CALLBACK, callback);  //注册闹钟回调函数
    rt_device_control(fd, RTC_GET_TIME, &rtc_tm); //获取rtc模块当前时间

    wkalrm.enabled = 1;
    wkalrm.time = rtc_tm;
    rtc_tm.tm_min -= 1;   //配置闹钟时间，注意enable需要置1才会使能alarm中断

    printf("alarm time %04d-%02d-%02d %02d:%02d:%02d\n",
           wkalrm.time.tm_year + 1900, wkalrm.time.tm_mon + 1, wkalrm.time.tm_mday,
           wkalrm.time.tm_hour, wkalrm.time.tm_min, wkalrm.time.tm_sec);

    rt_device_control(fd, RTC_SET_TIME, &rtc_tm);  //设置rtc当前时间
    rt_device_control(fd, RTC_SET_ALARM, &wkalrm); //设置rtc模块闹钟
    rt_device_control(fd, RTC_GET_ALARM, &wkalrm); //获取rtc模块闹钟

    rt_device_control(fd, RTC_GET_TIME, &rtc_tm); //获取时间
    rt_device_control(fd, RTC_IRQENABLE, &enable); //使能/失能rtc模块
    rt_device_close(fd); //关闭rtc模块
    return 0;
}
```

## hal层接口

在 rt_thread框架里面，一个比较完整的使用RTC模块的代码如下所示：

```
#include <stdio.h>
#include <sunxi_drv_rtc.h>
#include <sunxi_hal_rtc.h>

struct rtc_time rtc_tm;
struct rtc_wkalrm wkalrm;

static int cmd_hal_rtc(int argc, const char **argv)
{
    unsigned int enable = 0;

    hal_rtc_init();   //初始化RTC模块

    hal_rtc_register_callback(callback); //注册闹钟回调函数

    if (hal_rtc_gettime(&rtc_tm))
    {
        printf("sunxi rtc gettime error\n");
    }

    wkalrm.enabled = 1;
    wkalrm.time = rtc_tm;
    rtc_tm.tm_min -= 1;  //设置闹钟参数

    printf("alarm time %04d-%02d-%02d %02d:%02d:%02d\n",
           wkalrm.time.tm_year + 1900, wkalrm.time.tm_mon + 1, wkalrm.time.tm_mday,
           wkalrm.time.tm_hour, wkalrm.time.tm_min, wkalrm.time.tm_sec);

    if (hal_rtc_settime(&rtc_tm))  //设置rtc时间
    {
        printf("sunxi rtc settime error\n");
    }

    if (hal_rtc_setalarm(&wkalrm)) //设置rtc闹钟时间
    {
        printf("sunxi rtc setalarm error\n");
    }

    if (hal_rtc_getalarm(&wkalrm))  //获取rtc闹钟时间
    {
        printf("sunxi rtc getalarm error\n");
    }

    if (hal_rtc_gettime(&rtc_tm)) //获取rtc模块时间
    {
        printf("sunxi rtc gettime error\n");
    }

    //if do hal_rtc_alarm_irq_enable and hal_rtc_uninit, alarm will not work
    hal_rtc_alarm_irq_enable(enable); //使能/使能rtc闹钟中断

    hal_rtc_uninit(); //失能时钟
    return 0;
}
```

## 重要数据结构

RTC模块有两个重要的数据结构，下面对其进行讲解。

### RTC_TIME

该结构体用来保存RTC模块的时间，具体如下所示：

```
struct rtc_time
{
    int tm_sec; //秒
    int tm_min; //分
    int tm_hour; //时
    int tm_mday; //天
    int tm_mon; //月
    int tm_year; //年
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
```

在设置RTC时钟的时候需要填充该结构体，并且传给settime函数。而在获取时间的时候需要传入该结构体指针，gettime会填充该结构体。

主要注意的是，tm_year的值为真实的year减去1900, 而tm_mon为真实的mon减去1。所以在配置时间的时候需要year减去1900，mon减去1；在获取时间的时候相反；

### RTC_WKALARM

该结构体用来保存RTC的闹钟时间，具体如下所示：

```
struct rtc_wkalrm
{
    unsigned char enabled;  /* 0 = alarm disabled, 1 = alarm enabled */
    unsigned char pending;  /* 0 = alarm not pending, 1 = alarm pending */
    struct rtc_time time;   /* time the alarm is set to */
};
```

在设置RTC闹钟的时候需要填充该结构体，并且传给setalarm函数。而在获取时间的时候需要传入该结构体指针，getalarm会填充该结构体。

## 注意事项

RTC模块记录的时间范围为1900-2097年。
