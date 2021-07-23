#ifndef _SYS_TIME_H_MELIS_
#define _SYS_TIME_H_MELIS_
#include <typedef.h>
#include <kconfig.h>
#include <kernel/ktype.h>
/* #include <csp.h> */

/*  number of nsec per usec */
#define NSEC_PER_USEC    1000

/*  number of microseconds per millisecond */
#define USEC_PER_MSEC    1000

/*  number of milliseconds per second */
#define MSEC_PER_SEC     1000

#define ALARM_INTERRUPT_NORMAL          0
#define ALARM_INTERRUPT_EXTNMI          1
#define MELIS_TICK_BASE                 (MSEC_PER_SEC / CONFIG_HZ)   // ms
#define TMR_SHOT_MODE_ONLYONE           CSP_TMRC_TMR_MODE_ONE_SHOOT  //Timer will not automatically restart when it expires
#define TMR_SHOT_MODE_PERIODIC          CSP_TMRC_TMR_MODE_CONTINU    //Timer will     automatically restart when it expires
/* typedef CSP_TMRC_TmrMode_t              __hw_tmr_mode_e; */

//hardware timer type for request timer
/* typedef CSP_TMRC_TmrType_t      __hw_tmr_type_t; */

typedef struct __DATE_T
{
    __u16   year;
    __u8    month;
    __u8    day;
} __awos_date_t;

typedef struct __TIME_T
{
    __u8    hour;
    __u8    minute;
    __u8    second;
} __awos_time_t;

typedef enum __TMR_CNTR_STAT
{
    TMR_CNT_STAT_INVALID = 0,
    TMR_CNT_STAT_STOP   = 1,
    TMR_CNT_STAT_RUN    = 2,
    TMR_CNT_STAT_PAUSE  = 3,
    TMR_CNT_STAT_
} __tmr_cntr_stat_e;

typedef enum __RTC_INT_TYPE
{
    RTC_INT_TYPE_ALARM,     /* alarm interrupt  */
    RTC_INT_TYPE_TIMER,     /* timer interrupt  */
    RTC_INT_TYPE_CNTER,     /* conter interrupt */
    RTC_INT_TYPE_
} __rtc_int_type_e;

typedef enum __RTC_DEVICE_CMD_SET
{
    RTC_CMD_GET_TIME,       /* get current time */
    RTC_CMD_SET_TIME,       /* set current time */
    RTC_CMD_GET_DATE,       /* get current date */
    RTC_CMD_SET_DATE,       /* set current date */
    RTC_CMD_REQUEST_ALARM,  /* request alarm    */
    RTC_CMD_RELEASE_ALARM,  /* release alarm    */
    RTC_CMD_START_ALARM,    /* start alarm      */
    RTC_CMD_STOP_ALARM,     /* stop alarm       */
    RTC_CMD_QUERY_ALARM,    /* query alarm      */
    RTC_CMD_QUERY_INT,      /* qurey rtc interrupt  */
    RTC_CMD_SETCLOCKOUT,    /* set rtc clock out*/
    RTC_CMD_
} __rtc_device_cmd_set_e;

__s32 TIME_Init(void);
__s32 TIME_Exit(void);
__s32 TMR_Init(void);
__s32 TMR_Exit(void);
__s32 rtc_init(void);
__s32 rtc_exit(void);
__s32 CNTR_Init(void);
__s32 CNTR_Exit(void);
__s32 RTC_QueryInt(__s32 type);
__s32 TIME_KRNLTickInit(void);
__s32 TIME_SetKRNLTick(__pISR_t kerntick, __u32 period);
__s32 TIME_SetSTMRTick(__pISR_t stmrtick, __u32 period);

/* __hdle esTIME_RequestTimer(__hw_tmr_type_t *pType, __pCBK_t pHdlr, void *pArg, char *pUsr); */
__s32 esTIME_ReleaseTimer(__hdle hTmr);
/* __s32 esTIME_StartTimer(__hdle hTmr, __u32 nPeriod, __hw_tmr_mode_e nMode); */
__s32 esTIME_StopTimer(__hdle hTmr);
__u32 esTIME_QuerryTimer(__hdle hTmr);
__s32 esTIME_GetTime(__awos_time_t *time);
__s32 esTIME_SetTime(__awos_time_t *time);
__s32 esTIME_GetDate(__awos_date_t *date);
__s32 esTIME_SetDate(__awos_date_t *date);
__hdle esTIME_RequestAlarm(__u32 mode);
__s32 esTIME_ReleaseAlarm(__hdle alarm);
__s32 esTIME_StartAlarm(__hdle alarm, __u32 time);
__s32 esTIME_StopAlarm(__hdle alarm);
__u32 esTIME_QuerryAlarm(__hdle alarm);
__hdle esTIME_RequestCntr(__pCB_ClkCtl_t cb, char *pUsr);
__s32 esTIME_ReleaseCntr(__hdle hCntr);
__s32 esTIME_StartCntr(__hdle hCntr);
__s32 esTIME_StopCntr(__hdle hCntr);
__s32 esTIME_PauseCntr(__hdle hCntr);
__s32 esTIME_ContiCntr(__hdle hCntr);
__s32 esTIME_SetCntrValue(__hdle hCntr, __u32 value);
__u32 esTIME_QuerryCntr(__hdle hCntr);
__s32 esTIME_SetCntrPrescale(__hdle hCntr, __s32 prescl);
__s32 esTIME_QuerryCntrStat(__hdle hCntr);

#endif  //define _SYS_TIME_H_MELIS_
