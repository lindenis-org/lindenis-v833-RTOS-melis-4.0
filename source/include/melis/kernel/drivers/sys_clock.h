#ifndef __SYS_CLOCK__
#define __SYS_CLOCK__
#include <typedef.h>
#include <kernel/ktype.h>

//command for call-back function of clock change
typedef enum __CLK_CMD
{
    CLK_CMD_SCLKCHG_REQ = 0,    //command for notify that clock will change
    CLK_CMD_SCLKCHG_DONE,       //command for notify that clock change finish
    CLK_CMD_
} __clk_cmd_t;

//command or status of clock on/off
typedef enum __CLK_ONOFF
{
    CLK_OFF = 0,                //clock off
    CLK_ON = 1,                 //clock on
} __CLK_onoff_t;

//status of clock, busy or free
typedef enum __CLK_FREE
{
    CLK_FREE = 0,               //clock is not used
    CLK_USED = 1,               //clock is used by someone
} __clk_free_t;

typedef struct __WAKEUP_SYS_SOURCE
{
    __u8    touch_panel;
    __u8    ext_nmi;
    __u8    keyboard;
    __u8    usb;
    __u8    alarm;
    __u8    lradc;
} __wakeup_sys_source;

typedef enum __CLK_MODE
{
    CLK_MODE_0 = 0,               //clock mode 0
    CLK_MODE_1 = 1,               //clock mode 1
    CLK_MODE_2 = 2,               //clock mode 2
    CLK_MODE_3 = 3,               //clock mode 3
} __clk_mode_t;

__hdle esCLK_OpenMclk(__u32 nMclkNo);
__s32 esCLK_CloseMclk(__hdle hMclk);
__s32 esCLK_GetMclkSrc(__hdle hMclk);
__s32 esCLK_GetMclkDiv(__hdle hMclk);
void  esCLK_SysInfo(const char *name);
void  esCLK_ModInfo(const char *name);
__s32 esCLK_SetSrcFreq(__u32 nSclkNo, __u32 nFreq);
__s32 esCLK_MclkRegCb(__u32 nMclkNo, __pCB_ClkCtl_t pCb);
__s32 esCLK_MclkUnregCb(__u32 nMclkNo, __pCB_ClkCtl_t pCb);
__s32 esCLK_SetMclkSrc(__hdle hMclk, __s32 nSclkNo);
__s32 esCLK_SetMclkDiv(__hdle hMclk, __s32 nDiv);
__s32 esCLK_MclkOnOff(__hdle hMclk, __s32 bOnOff);
__s32 esCLK_MclkReset(__hdle hMclk, __s32 bReset);
__u32 esCLK_GetSrcFreq(__u32 nSclkNo);
__s32 CLK_Init(void);

#endif
