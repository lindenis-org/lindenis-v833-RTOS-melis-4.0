/*
 * =====================================================================================
 *
 *       Filename:  kapi.h
 *
 *    Description:  melis system api for user module.
 *
 *        Version:  Melis3.0
 *         Create:  2018-01-11 20:29:43
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-01 17:28:06
 *
 * =====================================================================================
 */

#ifndef  __MELIS_API_H__
#define  __MELIS_API_H__
#include <typedef.h>
#include "kmsg.h"
#include "boot/boot.h"
#include "drivers/sys_device.h"
#include "drivers/sys_pwm.h"
#include "drivers/sys_input.h"
#include "drivers/sys_charset.h"
#include "drivers/sys_clock.h"
#include "drivers/sys_dma.h"
#include "drivers/sys_fsys.h"
#include "drivers/sys_hwsc.h"
#include "drivers/sys_int.h"
#include "drivers/sys_modstub.h"
#include "drivers/sys_sios.h"
#include "drivers/sys_svc.h"
#include "drivers/sys_hid.h"

typedef void (*OS_TMR_CALLBACK)(void *parg);
/* definition for KMSG para max bytes len */
#define KMSG_PARA_BYTES_MAX           20
typedef struct
{
    __u32       size;                           /* ??Ϣ֡?Ĵ?С                                             */
    void       *msg;
    __u32       msgtype;                        /* user define                                              */
    __u32       msgsize;
    __u32       msglen;
    void       *data;                           /* ????buf?ĵ?ַ                                            */
    __u32       datsize;                        /* ????buf?Ĵ?С                                            */
    __u32       datatype;                       /* bit1~0: frame head/end flag 00: mid 01: end, 10: start, 11:start+end     */
    __u32       datalen;                        /* data?ĳ???                                               */
    __u8        reserve[12];
} __krnl_sktfrm_t;

#define KRNL_SKT_USR_IN                 0
#define KRNL_SKT_USR_OUT                1
#define KRNL_SKT_BUF_PHY_UNSEQ          (0x00000000)
#define KRNL_SKT_BUF_PHY_SEQ            (0x01000000)
#define KRNL_SKT_BUF_PHY_SEQMASK        (0x01000000)
#define KRNL_SKT_BUF_TYP_MASK           (0xff000000)

#define OS_PRIO_SELF                    0xFFu
#define OS_EVENT_NAME_SIZE              16
#define OS_TIMEOUT                      10u

#define OS_NO_ERR                       0u
#define KRNL_NO_ERR                     0u

#define OS_TICK_STEP_EN                 1                 /* Enable tick stepping feature for uC/OS-View                  */
#define OS_TICKS_PER_SEC                100               /* Set the number of ticks in one second                        */

/* EVENTFLAGS */
#define OS_FLAG_WAIT_CLR_ALL            0u                /* Wait for ALL    the bits specified to be CLR (i.e. 0)        */
#define OS_FLAG_WAIT_CLR_AND            0u
#define OS_FLAG_WAIT_CLR_ANY            1u                /* Wait for ANY of the bits specified to be CLR (i.e. 0)        */
#define OS_FLAG_WAIT_CLR_OR             1u
#define OS_FLAG_WAIT_SET_ALL            2u                /* Wait for ALL    the bits specified to be SET (i.e. 1)        */
#define OS_FLAG_WAIT_SET_AND            2u
#define OS_FLAG_WAIT_SET_ANY            3u                /* Wait for ANY of the bits specified to be SET (i.e. 1)        */
#define OS_FLAG_WAIT_SET_OR             3u
#define OS_FLAG_CONSUME              0x80u                /* Consume the flags if condition(s) satisfied                  */
#define OS_FLAG_CLR                     0u
#define OS_FLAG_SET                     1u

/* OS_XXX_PostOpt() OPTIONS */
#define OS_POST_OPT_NONE                 0x00u           /* NO option selected                                           */
#define OS_POST_OPT_BROADCAST            0x01u           /* Broadcast message to ALL tasks waiting                       */
#define OS_POST_OPT_FRONT                0x02u           /* Post to highest priority task waiting                        */
#define OS_POST_OPT_NO_SCHED             0x04u           /* Do not call the scheduler if this option is selected         */

#define OS_TASK_STAT_EN                 0
#define OS_TMR_CFG_NAME_SIZE            16               /* Determine the size of a timer name                       */

#define OS_ERR_EVENT_TYPE               1u
#define OS_ERR_PEND_ISR                 2u
#define OS_ERR_POST_NULL_PTR            3u
#define OS_ERR_PEVENT_NULL              4u
#define OS_ERR_POST_ISR                 5u
#define OS_ERR_QUERY_ISR                6u
#define OS_ERR_INVALID_OPT              7u
#define OS_ERR_TASK_WAITING             8u
#define OS_ERR_PDATA_NULL               9u

#define OS_TIMEOUT                     10u
#define OS_TASK_NOT_EXIST              11u
#define OS_ERR_EVENT_NAME_TOO_LONG     12u
#define OS_ERR_FLAG_NAME_TOO_LONG      13u
#define OS_ERR_TASK_NAME_TOO_LONG      14u
#define OS_ERR_PNAME_NULL              15u
#define OS_ERR_TASK_CREATE_ISR         16u
#define OS_ERR_PEND_LOCKED             17u

#define OS_MBOX_FULL                   20u

#define OS_TMR_OPT_NONE                0u          /*  No option selected                                           */
#define OS_TMR_OPT_ONE_SHOT            1u          /*  Timer will not automatically restart when it expires         */
#define OS_TMR_OPT_PERIODIC            2u          /*  Timer will     automatically restart when it expires         */
#define OS_TMR_OPT_CALLBACK            3u          /*  OSTmrStop() option to call 'callback' w/ timer arg.          */
#define OS_TMR_OPT_CALLBACK_ARG        4u          /*  OSTmrStop() option to call 'callback' w/ new   arg.          */
#define OS_TMR_OPT_NORMAL_MASK         0x0fu       /*  soft timer normal option mask                                */
#define OS_TMR_OPT_PRIO_MASK           0x30u       /*  mask of the priority soft timer                              */
#define OS_TMR_OPT_PRIO_LOW            0x00u       /*  low priority soft timer                                      */
#define OS_TMR_OPT_PRIO_HIGH           0x10u       /*  special soft timer, can be operated in isr, used be careful  */
#define OS_TMR_STATE_UNUSED            0u
#define OS_TMR_STATE_STOPPED           1u
#define OS_TMR_STATE_COMPLETED         2u
#define OS_TMR_STATE_RUNNING           3u

#define OS_DEL_NO_PEND                 0u
#define OS_DEL_ALWAYS                  1u

#define KRNL_Q_FULL                    30u
#define KRNL_Q_EMPTY                   31u

#define OS_PRIO_EXIST                  40u
#define OS_PRIO_ERR                    41u
#define OS_PRIO_INVALID                42u

#define OS_SEM_OVF                     50u

#define OS_TASK_DEL_ERR                60u
#define OS_TASK_DEL_IDLE               61u
#define OS_TASK_DEL_REQ                62u
#define OS_TASK_DEL_ISR                63u

#define OS_NO_MORE_TCB                 70u

#define OS_TIME_NOT_DLY                80u
#define OS_TIME_INVALID_MINUTES        81u
#define OS_TIME_INVALID_SECONDS        82u
#define OS_TIME_INVALID_MILLI          83u
#define OS_TIME_ZERO_DLY               84u

#define OS_TASK_SUSPEND_PRIO           90u
#define OS_TASK_SUSPEND_IDLE           91u

#define OS_TASK_RESUME_PRIO           100u
#define OS_TASK_NOT_SUSPENDED         101u


#define OS_ERR_NOT_MUTEX_OWNER        120u

#define OS_TASK_OPT_ERR               130u

#define OS_ERR_DEL_ISR                140u
#define OS_ERR_CREATE_ISR             141u

#define OS_FLAG_INVALID_PGRP          150u
#define OS_FLAG_ERR_WAIT_TYPE         151u
#define OS_FLAG_ERR_NOT_RDY           152u
#define OS_FLAG_INVALID_OPT           153u
#define KRNL_FLAG_GRP_DEPLETED        154u

#define OS_ERR_PIP_LOWER              160u

#define KRNL_priolevel0               0                   /* reserved for system(highest level)                           */
#define KRNL_priolevel1               1
#define KRNL_priolevel2               2
#define KRNL_priolevel3               3
#define KRNL_priolevel4               4
#define KRNL_priolevel5               5
#define KRNL_priolevel6               6
#define KRNL_priolevel7               7                   /* reserved for system(lowest level)                            */

#define EXEC_pidself                  0xff
#define EXEC_prioself                 0xff

/*wait process main function to return  */
#define EXEC_CREATE_WAIT_RET          (1<<0)

typedef struct  KNRL_STMR
{
    __u16           err;
} __krnl_stmr_t;

typedef struct os_flag_grp                          /* Event Flag Group                                         */
{
    __u8            OSFlagType;                     /* Should be set to OS_EVENT_TYPE_FLAG                      */
    void           *OSFlagWaitList;                 /* Pointer to first NODE of task waiting on event flag      */
    __krnl_flags_t  OSFlagFlags;                    /* 8, 16 or 32 bit flags                                    */
} __krnl_flag_grp_t;

typedef struct __EXEC_MGSEC
{
    char            magic[8];                       // "ePDK.exf"
    __u32           version;
    __u8            type;                           // 0: reserved
    __u32           heapaddr;                       //heap??32M?ռ??ڵ?λ?ã?ͨ????????1M(31*1024*1024)
    //????ַ????0????ô??ʾ???̲???Ҫ?ֲ??ѣ???????ϵͳ??
    __u32           heapsize;                       //heap?ĳ?ʼ??С
    __s32(*main)(void *p_arg);                      //pcb????һ?????????׵?ַ
    __u32           mtskstksize;                    /* ??һ????????stack    size                        */
    __u8            mtskprio;                       /* ??һ???????????ȼ?                               */
} __exec_mgsec_t;

typedef struct __krnl_q_data_t
{
    void         *OSMsg;                            /* Pointer to next message to be extracted from queue       */
    __u16         OSNMsgs;                          /* Number of messages in message queue                      */
    __u16         OSQSize;                          /* Size of message queue                                    */
    __u16         OSEventTbl[16];                   /* List of tasks waiting for event to occur         */
    __u16         OSEventGrp;                       /* Group corresponding to tasks waiting for event to occur  */
} __krnl_q_data_t;

/* 设备操作函数??? */
typedef enum enum_EPDK_VER_TYPE
{
    EPDK_VER,
    EPDK_VER_OS,
    EPDK_VER_CHIP,
    EPDK_VER_PID,
    EPDK_VER_SID,
    EPDK_VER_BID,
    EPDK_VER_CHIP_SUB,
} __epdk_ver_type_t;

//KRNL
void esKRNL_TimeDly(__u16 ticks);
void esKRNL_TimeSet(__u32 ticks);
void esKRNL_SemPend(__hdle sem, __u16 timeout, __u8 *err);
void esKRNL_SemSet(__hdle pevent, __u16 cnt, __u8 *err);
void esKRNL_MutexPend(__hdle pevent, __u16 timeout, __u8 *err);
void esKRNL_FlagNameSet(__hdle pgrp, __u8 *pname, __u8 *err);
void esKRNL_SchedLock(void);
void esKRNL_SchedUnlock(void);
unsigned long esKRNL_InterruptDisable(void);
void esKRNL_InterruptEnable(unsigned long level);

void esKRNL_TaskPrefEn(__u32 en);
void esKRNL_MemLeakChk(__u32 en);
void esKRNL_DumpStack(void);

__s8 esKRNL_TDel(__u32 prio);
__s8 esKRNL_TDelReq(__u32 prio_ex);
__s8 esKRNL_TaskDel(__u32 prio);
__s8 esKRNL_TaskDelReq(__u32 prio_ex);
__u8 esKRNL_TaskResume(__u32 prio);
__u8 esKRNL_TaskSuspend(__u32 prio);
__u8 esKRNL_TimeDlyResume(__u32 prio_ex);
__u8 esKRNL_TaskQuery(__u32 prio, __krnl_tcb_t *p_task_data);
__u8 esKRNL_SemPost(__hdle sem);
__u8 esKRNL_SemQuery(__hdle sem, OS_SEM_DATA *p_sem_data);
__u8 esKRNL_MboxPost(__hdle pevent, __u32 msg);
__u8 esKRNL_MboxPostOpt(__hdle pevent, __u32 msg, __u8 opt);
__u8 esKRNL_MboxQuery(__hdle pevent, void *p_mbox_data);
__u8 esKRNL_MutexPost(__hdle pevent);
__u8 esKRNL_QFlush(__hdle pevent);
__u8 esKRNL_QPost(__hdle pevent, __u32 msg);
__u8 esKRNL_QPostFront(__hdle pevent, void *msg);
__u8 esKRNL_QPostOpt(__hdle pevent, void *msg, __u8 opt);
__u8 esKRNL_QQuery(__hdle pevent, __krnl_q_data_t *p_q_data);
__u8 esKRNL_TmrStateGet(__hdle ptmr);
__u8 esKRNL_FlagNameGet(__hdle pgrp, __u8 *pname, __u8 *err);
__u8 esKSRV_GetSocID(void);

__u32 esKRNL_TCreate(void (*entry)(void *p_arg), void *p_arg, __u32 stksize, __u16 id_priolevel);
__u32 esKRNL_TimeGet(void);
__u32 esKRNL_MboxAccept(__hdle pevent);
__u32 esKRNL_MboxDel(__hdle pevent, __u8 opt, __u8 *err);
__u32 esKRNL_MboxPend(__hdle pevent, __u16 timeout, __u8 *err);
__u32 esKRNL_MutexDel(__hdle pevent, __u8 opt, __u8 *err);
__u32 esKRNL_QAccept(__hdle pevent, __u8 *err);
__u32 esKRNL_QPend(__hdle pevent, __u16 timeout, __u8 *err);
__u32 esKRNL_FlagAccept(__hdle pgrp, __u32 flags, __u8 wait_type, __u8 *err);
__u32 esKRNL_FlagDel(__hdle pgrp, __u8 opt, __u8 *err);
__u32 esKRNL_FlagPend(__hdle pgrp, __u32 flags, __u32 waittype_timeout, __u8 *err);
__u32 esKRNL_FlagPendGetFlagsRdy(void);
__u32 esKRNL_FlagPost(__hdle pgrp, __u32 flags, __u8 opt, __u8 *err);
__u32 esKRNL_FlagQuery(__hdle pgrp, __u8 *err);
__u32 esKRNL_TmrRemainGet(__hdle ptmr);
__u32 esKRNL_GetTIDCur(void);
__u32 esKRNL_Time(void);

__s32 esKRNL_TmrDel(__hdle ptmr);
__s32 esKRNL_TmrStart(__hdle ptmr);
__s32 esKRNL_TmrStop(__hdle ptmr, __s8 opt, void *callback_arg);
__s32 esKRNL_CallBack(__pCBK_t cb, void *arg);
__s32 esKRNL_SktDel(__hdle skt, __u8 opt);
__s32 esKRNL_SktPost(__hdle skt, __u8 user, __krnl_sktfrm_t *frm);

__u16 esKRNL_SemAccept(void *psem);
__s16 esKRNL_TmrError(__hdle ptmr);
__u16 esKRNL_Version(void);

__hdle esKRNL_MutexCreate(__u8 prio, __u8 *err);
__hdle esKRNL_SktCreate(__u32 depth, __u32 dbuf_attr, __u32 mbuf_attr);
__hdle esKRNL_SemCreate(__u16 cnt);
__hdle esKRNL_SemDel(__hdle sem, __u8 opt, __u8 *err);
__hdle esKRNL_MboxCreate(__u32 msg);
__hdle esKRNL_QCreate(__u16 size);
__hdle esKRNL_QDel(__hdle pevent, __u8 opt, __u8 *err);
__hdle esKRNL_FlagCreate(__u32 flags, __u8 *err);
__hdle esKRNL_TmrCreate(__u32 period, __u8 opt, OS_TMR_CALLBACK callback, void *callback_arg);
__pCBK_t esKRNL_GetCallBack(__pCBK_t cb);
__krnl_sktfrm_t *esKRNL_SktPend(__hdle skt, __u8 user, __u32 timeout);
__krnl_sktfrm_t *esKRNL_SktAccept(__hdle skt, __u8 user);

//KRSV
void  esKSRV_Reset(void);
void  esKSRV_PowerOff(void);
__s32 esKSRV_SysInfo(void);
__s32 esKSRV_SendMsgEx(void *msg);
__u32 esKSRV_GetMsg(void);
__u32 esKSRV_GetVersion(__epdk_ver_type_t type);
__u32 esKSRV_Random(__u32 max);
__s32 esKSRV_SendMsg(__u32 msgid, __u32 prio);
__s32 esKSRV_EnableWatchDog(void);
__s32 esKSRV_DisableWatchDog(void);
__s32 esKSRV_ClearWatchDog(void);
__s32 esKSRV_EnableWatchDogSrv(void);
__s32 esKSRV_DisableWatchDogSrv(void);
__s32 esKSRV_memcpy(void *pdest, const void *psrc, size_t size);
__u32 esKSRV_GetLowMsg(void);
__u32 esKSRV_GetHighMsg(void);
__s32 esKSRV_GetPara(__u32 type, void *format, void *para);
__s32 esKSRV_GetDramCfgPara(__dram_para_t *drampara);
__s32 esKSRV_memset(void *pmem, __u8 value, size_t size);
__s32 esKSRV_GetAddPara(__ksrv_add_para *ksrv_add_para);
__u32 esKSRV_close_logo(void);
__u32 esKSRV_release_logo_buf(void);

// EXEC
__u8 esEXEC_PCreate(const char *filename, void *p_arg, __u32 mode, __u32 *ret);
__s8 esEXEC_PDel(__u8 id);
__s8 esEXEC_PDelReq(__u8 pid);
__s8 esEXEC_Run(const char *pfilename, void *p_arg, __u32 mode, __u32 *ret);

// RESM
void *esRESM_ROpen(const char *file, const char *mode);
__s32 esRESM_RClose(__resm_rsb_t *res);
__u32 esRESM_RRead(void *pdata, __u32 size, __u32 n, __resm_rsb_t *res);
__s32 esRESM_RSeek(__resm_rsb_t *res, __s32 Offset, __s32 Whence);

typedef struct
{
    void *SWIHandler_SIOS          ;
    void *SWIHandler_KRNL          ;
    void *SWIHandler_MEMS          ;
    void *SWIHandler_FSYS          ;
    void *SWIHandler_EXEC          ;
    void *SWIHandler_MODS          ;
    void *SWIHandler_RESM          ;
    void *SWIHandler_INT           ;
    void *SWIHandler_DMA           ;
    void *SWIHandler_TIME          ;
    void *SWIHandler_IPIPE         ;
    void *SWIHandler_PWRS          ;
    void *SWIHandler_ERRS          ;
    void *SWIHandler_SVC           ;
    void *SWIHandler_DEV           ;
    void *SWIHandler_KSRV          ;
    void *SWIHandler_PINS          ;
    void *SWIHandler_CLK           ;
    void *SWIHandler_MEM           ;
    void *SWIHandler_HID           ;
    void *SWIHandler_PWM           ;
    void *SWIHandler_CHS           ;
    void *SWIHandler_MSTUB         ;
    void *SWIHandler_INPUT         ;
    void *SWIHandler_CONFIG        ;
    void *SWIHandler_PTHREAD       ;
} SWIHandler_SWIT_t;

typedef struct
{
    __pSWI_t esSIOS_getchar    ;
    __pSWI_t esSIOS_gets       ;
    __pSWI_t esSIOS_putarg     ;
    __pSWI_t esSIOS_putstr     ;
    __pSWI_t esSIOS_setbaud    ;
} SWIHandler_SIOS_t;

typedef struct
{
    __pSWI_t esKRNL_TCreate             ;
    __pSWI_t esKRNL_TDel                ;
    __pSWI_t esKRNL_TDelReq             ;
    __pSWI_t esKRNL_GetPrio             ;
    __pSWI_t esKRNL_FreePrio            ;
    __pSWI_t esKRNL_TaskChangePrio      ;
    __pSWI_t esKRNL_TaskDel             ;
    __pSWI_t esKRNL_TaskDelReq          ;
    __pSWI_t esKRNL_TaskNameGet         ;
    __pSWI_t esKRNL_TaskNameSet         ;
    __pSWI_t esKRNL_TaskResume          ;
    __pSWI_t esKRNL_TaskSuspend         ;
    __pSWI_t esKRNL_TaskStkChk          ;
    __pSWI_t esKRNL_TaskQuery           ;
    __pSWI_t esKRNL_TimeDly             ;
    __pSWI_t esKRNL_TimeDlyHMSM         ;
    __pSWI_t esKRNL_TimeDlyResume       ;
    __pSWI_t esKRNL_TimeGet             ;
    __pSWI_t esKRNL_TimeSet             ;
    __pSWI_t esKRNL_SemAccept           ;
    __pSWI_t esKRNL_SemCreate           ;
    __pSWI_t esKRNL_SemDel              ;
    __pSWI_t esKRNL_SemPend             ;
    __pSWI_t esKRNL_SemPost             ;
    __pSWI_t esKRNL_SemQuery            ;
    __pSWI_t esKRNL_SemSet              ;
    __pSWI_t esKRNL_MboxAccept          ;
    __pSWI_t esKRNL_MboxCreate          ;
    __pSWI_t esKRNL_MboxDel             ;
    __pSWI_t esKRNL_MboxPend            ;
    __pSWI_t esKRNL_MboxPost            ;
    __pSWI_t esKRNL_MboxPostOpt         ;
    __pSWI_t esKRNL_MboxQuery           ;
    __pSWI_t esKRNL_MutexAccept         ;
    __pSWI_t esKRNL_MutexCreate         ;
    __pSWI_t esKRNL_MutexDel            ;
    __pSWI_t esKRNL_MutexPend           ;
    __pSWI_t esKRNL_MutexPost           ;
    __pSWI_t esKRNL_MutexQuery          ;
    __pSWI_t esKRNL_QAccept             ;
    __pSWI_t esKRNL_QCreate             ;
    __pSWI_t esKRNL_QDel                ;
    __pSWI_t esKRNL_QFlush              ;
    __pSWI_t esKRNL_QPend               ;
    __pSWI_t esKRNL_QPost               ;
    __pSWI_t esKRNL_QPostFront          ;
    __pSWI_t esKRNL_QPostOpt            ;
    __pSWI_t esKRNL_QQuery              ;
    __pSWI_t esKRNL_FlagAccept          ;
    __pSWI_t esKRNL_FlagCreate          ;
    __pSWI_t esKRNL_FlagDel             ;
    __pSWI_t esKRNL_FlagNameGet         ;
    __pSWI_t esKRNL_FlagNameSet         ;
    __pSWI_t esKRNL_FlagPend            ;
    __pSWI_t esKRNL_FlagPendGetFlagsRdy ;
    __pSWI_t esKRNL_FlagPost            ;
    __pSWI_t esKRNL_FlagQuery           ;
    __pSWI_t esKRNL_TmrCreate           ;
    __pSWI_t esKRNL_TmrDel              ;
    __pSWI_t esKRNL_TmrStart            ;
    __pSWI_t esKRNL_TmrStop             ;
    __pSWI_t esKRNL_TmrRemainGet        ;
    __pSWI_t esKRNL_TmrStateGet         ;
    __pSWI_t esKRNL_TmrError            ;
    __pSWI_t esKRNL_Version             ;
    __pSWI_t esKRNL_SchedLock           ;
    __pSWI_t esKRNL_SchedUnlock         ;
    __pSWI_t esKRNL_GetCallBack         ;
    __pSWI_t esKRNL_CallBack            ;
    __pSWI_t esKRNL_GetTIDCur           ;
    __pSWI_t esKRNL_SktCreate           ;
    __pSWI_t esKRNL_SktDel              ;
    __pSWI_t esKRNL_SktPend             ;
    __pSWI_t esKRNL_SktPost             ;
    __pSWI_t esKRNL_SktAccept           ;
    __pSWI_t esKRNL_SktFlush            ;
    __pSWI_t esKRNL_SktError            ;
    __pSWI_t esKRNL_Time                ;
#if (OS_TASK_STAT_EN > 0) && (OS_CPU_HOOKS_EN > 0)
    __pSWI_t esKRNL_CPUStatStart        ;
    __pSWI_t esKRNL_CPUStatStop         ;
    __pSWI_t esKRNL_CPUStatReport       ;
#endif
    __pSWI_t esKRNL_TaskPrefEn          ;
    __pSWI_t esKRNL_MemLeakChk          ;
    __pSWI_t esKRNL_DumpStack           ;
} SWIHandler_KRNL_t;

typedef struct
{
    __pSWI_t esMEMS_Palloc             ;
    __pSWI_t esMEMS_Pfree              ;
    __pSWI_t esMEMS_VA2PA              ;
    __pSWI_t esMEMS_VMCreate           ;
    __pSWI_t esMEMS_VMDelete           ;
    __pSWI_t esMEMS_HeapCreate         ;
    __pSWI_t esMEMS_HeapDel            ;
    __pSWI_t esMEMS_Malloc             ;
    __pSWI_t esMEMS_Mfree              ;
    __pSWI_t esMEMS_Realloc            ;
    __pSWI_t esMEMS_Calloc             ;
    __pSWI_t esMEMS_Balloc             ;
    __pSWI_t esMEMS_Bfree              ;
    __pSWI_t esMEMS_Info               ;
    __pSWI_t esMEMS_CleanDCache        ;
    __pSWI_t esMEMS_CleanFlushDCache   ;
    __pSWI_t esMEMS_CleanFlushCache    ;
    __pSWI_t esMEMS_FlushDCache        ;
    __pSWI_t esMEMS_FlushICache        ;
    __pSWI_t esMEMS_FlushCache         ;
    __pSWI_t esMEMS_CleanDCacheRegion  ;
    __pSWI_t esMEMS_CleanFlushDCacheRegion ;
    __pSWI_t esMEMS_CleanFlushCacheRegion  ;
    __pSWI_t esMEMS_FlushDCacheRegion  ;
    __pSWI_t esMEMS_FlushICacheRegion  ;
    __pSWI_t esMEMS_FlushCacheRegion   ;
    __pSWI_t esMEMS_VMalloc            ;
    __pSWI_t esMEMS_VMfree             ;
    __pSWI_t esMEMS_FreeMemSize        ;
    __pSWI_t esMEMS_TotalMemSize       ;
    __pSWI_t esMEMS_LockICache         ;
    __pSWI_t esMEMS_LockDCache         ;
    __pSWI_t esMEMS_UnlockICache       ;
    __pSWI_t esMEMS_UnlockDCache       ;
    __pSWI_t esMEMS_GetIoVAByPA        ;
    __pSWI_t esMEMS_PhyAddrConti       ;
    __pSWI_t esMEMS_CleanInvalidateCacheAll;
} SWIHandler_MEMS_t;

typedef struct
{
    __pSWI_t esFSYS_opendir         ;
    __pSWI_t esFSYS_closedir        ;
    __pSWI_t esFSYS_readdir         ;
    __pSWI_t esFSYS_rewinddir       ;
    __pSWI_t esFSYS_mkdir           ;
    __pSWI_t esFSYS_rmdir           ;
    __pSWI_t esFSYS_remove          ;
    __pSWI_t esFSYS_rename          ;
    __pSWI_t esFSYS_fopen           ;
    __pSWI_t esFSYS_fclose          ;
    __pSWI_t esFSYS_fread           ;
    __pSWI_t esFSYS_fwrite          ;
    __pSWI_t esFSYS_fseek           ;
    __pSWI_t esFSYS_ftell           ;
    __pSWI_t esFSYS_fseekex         ;
    __pSWI_t esFSYS_ftellex         ;
    __pSWI_t esFSYS_fioctrl         ;
    __pSWI_t esFSYS_ferror          ;
    __pSWI_t esFSYS_ferrclr         ;
    __pSWI_t esFSYS_premove         ;
    __pSWI_t esFSYS_prename         ;
    __pSWI_t esFSYS_popen           ;
    __pSWI_t esFSYS_pclose          ;
    __pSWI_t esFSYS_pread           ;
    __pSWI_t esFSYS_pwrite          ;
    __pSWI_t esFSYS_pioctrl         ;
    __pSWI_t esFSYS_perror          ;
    __pSWI_t esFSYS_perrclr         ;
    __pSWI_t esFSYS_mntparts        ;
    __pSWI_t esFSYS_umntparts       ;
    __pSWI_t esFSYS_fsreg           ;
    __pSWI_t esFSYS_fsunreg         ;
    __pSWI_t esFSYS_pdreg           ;
    __pSWI_t esFSYS_pdunreg         ;
    __pSWI_t esFSYS_partfslck       ;
    __pSWI_t esFSYS_partfsunlck     ;
    __pSWI_t esFSYS_mntfs           ;
    __pSWI_t esFSYS_umntfs          ;
    __pSWI_t esFSYS_format          ;
    __pSWI_t esFSYS_statfs          ;
    __pSWI_t esFSYS_fsdbg           ;
    __pSWI_t esFSYS_fsync           ;
    __pSWI_t esFSYS_fstat           ;
    __pSWI_t esFSYS_statpt          ;
    __pSWI_t esFSYS_setfs           ;
    __pSWI_t esFSYS_getfscharset    ;
    /* main for POSIX interface support */
    __pSWI_t esFSYS_file2fd         ;
    __pSWI_t esFSYS_fd2file         ;
    __pSWI_t esFSYS_open            ;
    /* main for update media file information performance */
    __pSWI_t esFSYS_querypartupdateflag;
    __pSWI_t esFSYS_clearpartupdateflag;
    __pSWI_t esFSYS_ftruncate;
} SWIHandler_FSYS_t;

typedef struct
{
    __pSWI_t esEXEC_PCreate       ;
    __pSWI_t esEXEC_PDel          ;
    __pSWI_t esEXEC_PDelReq       ;
    __pSWI_t esEXEC_Run           ;
} SWIHandler_EXEC_t;


typedef struct
{
    __pSWI_t esMODS_MInstall              ;
    __pSWI_t esMODS_MUninstall            ;
    __pSWI_t esMODS_MOpen                 ;
    __pSWI_t esMODS_MClose                ;
    __pSWI_t esMODS_MRead                 ;
    __pSWI_t esMODS_MWrite                ;
    __pSWI_t esMODS_MIoctrl               ;
} SWIHandler_MODS_t;

typedef struct
{
    __pSWI_t esRESM_ROpen                ;
    __pSWI_t esRESM_RClose               ;
    __pSWI_t esRESM_RRead                ;
    __pSWI_t esRESM_RSeek                ;
} SWIHandler_RESM_t;

typedef struct
{
    __pSWI_t esINT_InsISR            ;
    __pSWI_t esINT_UniISR            ;
    __pSWI_t esINT_InsFIR            ;
    __pSWI_t esINT_UniFIR            ;
    __pSWI_t esINT_SetIRQPrio        ;
    __pSWI_t esINT_DisableINT        ;
    __pSWI_t esINT_EnableINT         ;
} SWIHandler_INT_t;

typedef struct
{
    __pSWI_t esDMA_Request          ;
    __pSWI_t esDMA_Release          ;
    __pSWI_t esDMA_Setting          ;
    __pSWI_t esDMA_Start            ;
    __pSWI_t esDMA_Restart          ;
    __pSWI_t esDMA_Stop             ;
    __pSWI_t esDMA_QueryStat        ;
    __pSWI_t esDMA_QuerySrc         ;
    __pSWI_t esDMA_QueryDst         ;
    __pSWI_t esDMA_QueryRestCount   ;
    __pSWI_t esDMA_ChangeMode       ;
    __pSWI_t esDMA_EnableINT        ;
    __pSWI_t esDMA_DisableINT       ;
    __pSWI_t esDMA_RegDmaHdler      ;
    __pSWI_t esDMA_UnregDmaHdler    ;
    __pSWI_t esDMA_Information      ;
} SWIHandler_DMA_t;

typedef struct
{
    __pSWI_t esTIME_RequestTimer        ;
    __pSWI_t esTIME_ReleaseTimer        ;
    __pSWI_t esTIME_StartTimer          ;
    __pSWI_t esTIME_StopTimer           ;
    __pSWI_t esTIME_QuerryTimer         ;

    __pSWI_t esTIME_GetTime             ;
    __pSWI_t esTIME_SetTime             ;
    __pSWI_t esTIME_GetDate             ;
    __pSWI_t esTIME_SetDate             ;
    __pSWI_t esTIME_RequestAlarm        ;
    __pSWI_t esTIME_ReleaseAlarm        ;
    __pSWI_t esTIME_StartAlarm          ;
    __pSWI_t esTIME_StopAlarm           ;
    __pSWI_t esTIME_QuerryAlarm         ;

    __pSWI_t esTIME_RequestCntr         ;
    __pSWI_t esTIME_ReleaseCntr         ;
    __pSWI_t esTIME_StartCntr           ;
    __pSWI_t esTIME_StopCntr            ;
    __pSWI_t esTIME_PauseCntr           ;
    __pSWI_t esTIME_ContiCntr           ;
    __pSWI_t esTIME_SetCntrValue        ;
    __pSWI_t esTIME_QuerryCntr          ;
    __pSWI_t esTIME_SetCntrPrescale     ;
    __pSWI_t esTIME_QuerryCntrStat      ;

} SWIHandler_TIME_t;


typedef struct
{
    __pSWI_t esIPIPE_Open          ;
    __pSWI_t esIPIPE_Close         ;
    __pSWI_t esIPIPE_Request       ;
    __pSWI_t esIPIPE_Release       ;
    __pSWI_t esIPIPE_Query         ;
    __pSWI_t esIPIPE_Reset         ;
    __pSWI_t esIPIPE_RX            ;
    __pSWI_t esIPIPE_TX            ;
} SWIHandler_IPIPE_t;

typedef struct
{
    __pSWI_t esSVC_RegCreatePath        ;
    __pSWI_t esSVC_RegDeletePath        ;

    __pSWI_t esSVC_RegOpenNode         ;
    __pSWI_t esSVC_RegCloseNode        ;
    __pSWI_t esSVC_RegDeleteNode       ;

    __pSWI_t esSVC_RegCreateSet        ;
    __pSWI_t esSVC_RegDeleteSet        ;
    __pSWI_t esSVC_RegGetSetCount      ;
    __pSWI_t esSVC_RegGetFirstSet      ;
    __pSWI_t esSVC_RegGetNextSet       ;

    __pSWI_t esSVC_RegCreateKey        ;
    __pSWI_t esSVC_RegDeleteKey        ;
    __pSWI_t esSVC_RegGetKeyCount      ;
    __pSWI_t esSVC_RegGetFirstKey      ;
    __pSWI_t esSVC_RegGetNextKey       ;
    __pSWI_t esSVC_RegSetKeyValue      ;
    __pSWI_t esSVC_RegGetKeyValue      ;

    __pSWI_t esSVC_RegIni2Reg          ;
    __pSWI_t esSVC_RegReg2Ini          ;
    __pSWI_t esSVC_RegSetRootPath      ;
    __pSWI_t esSVC_RegGetRootPath      ;
    __pSWI_t esSVC_RegGetError         ;
    __pSWI_t esSVC_RegClrError         ;
    // resource maangement sevices
    __pSWI_t esSVC_ResourceReq         ;
    __pSWI_t esSVC_ResourceRel         ;
} SWIHandler_SVC_t;

typedef struct
{
    __pSWI_t esDEV_Plugin               ;
    __pSWI_t esDEV_Plugout              ;
    __pSWI_t esDEV_DevReg               ;
    __pSWI_t esDEV_DevUnreg             ;
    __pSWI_t esDEV_Open                 ;
    __pSWI_t esDEV_Close                ;
    __pSWI_t esDEV_Read                 ;
    __pSWI_t esDEV_Write                ;
    __pSWI_t esDEV_Ioctl                ;
    __pSWI_t esDEV_Lock                 ;
    __pSWI_t esDEV_Unlock               ;
    __pSWI_t esDEV_Insmod               ;
    __pSWI_t esDEV_Unimod               ;
} SWIHandler_DEV_t;

typedef struct
{
    __pSWI_t esKSRV_SysInfo             ;
    __pSWI_t esKSRV_SendMsgEx           ;
    __pSWI_t esKSRV_GetMsg              ;
    __pSWI_t esKSRV_GetVersion          ;
    __pSWI_t esKSRV_Random              ;
    __pSWI_t esKSRV_Reset               ;
    __pSWI_t esKSRV_GetSocID            ;
    __pSWI_t esKSRV_PowerOff            ;
    __pSWI_t esKSRV_SendMsg             ;
    __pSWI_t esKSRV_GetTargetPara       ;
    __pSWI_t esKSRV_EnableWatchDog      ;
    __pSWI_t esKSRV_DisableWatchDog     ;
    __pSWI_t esKSRV_ClearWatchDog       ;
    __pSWI_t esKSRV_EnableWatchDogSrv   ;
    __pSWI_t esKSRV_DisableWatchDogSrv  ;
    __pSWI_t esKSRV_memcpy              ;
    __pSWI_t esKSRV_GetLowMsg           ;
    __pSWI_t esKSRV_GetHighMsg          ;
    __pSWI_t esKSRV_GetPara             ;
    __pSWI_t esKSRV_GetDramCfgPara      ;
    __pSWI_t esKSRV_memset              ;
    __pSWI_t esKSRV_GetAddPara          ;
    __pSWI_t esKSRV_close_logo          ;
    __pSWI_t esKSRV_release_logo_buf    ;
} SWIHandler_KSRV_t;

typedef struct
{
    __pSWI_t esPINS_PinGrpReq    ;
    __pSWI_t esPINS_PinGrpRel    ;
    __pSWI_t esPINS_GetPinGrpStat;
    __pSWI_t esPINS_GetPinStat   ;
    __pSWI_t esPINS_SetPinStat   ;
    __pSWI_t esPINS_SetPinIO     ;
    __pSWI_t esPINS_SetPinPull   ;
    __pSWI_t esPINS_SetPinDrive  ;
    __pSWI_t esPINS_ReadPinData  ;
    __pSWI_t esPINS_WritePinData ;
    __pSWI_t esPINS_EnbaleInt    ;
    __pSWI_t esPINS_DisbaleInt   ;
    __pSWI_t esPINS_QueryInt     ;
    __pSWI_t esPINS_SetIntMode   ;
    __pSWI_t esPINS_RegIntHdler  ;
    __pSWI_t esPINS_UnregIntHdler;
    __pSWI_t esPINS_ClearPending ;
} SWIHandler_PINS_t;

typedef struct
{
    __pSWI_t esCLK_SetSrcFreq   ;
    __pSWI_t esCLK_GetSrcFreq   ;
    __pSWI_t esCLK_OpenMclk     ;
    __pSWI_t esCLK_CloseMclk    ;
    __pSWI_t esCLK_MclkRegCb    ;
    __pSWI_t esCLK_MclkUnregCb  ;
    __pSWI_t esCLK_SetMclkSrc   ;
    __pSWI_t esCLK_GetMclkSrc   ;
    __pSWI_t esCLK_SetMclkDiv   ;
    __pSWI_t esCLK_GetMclkDiv   ;
    __pSWI_t esCLK_MclkOnOff    ;
    __pSWI_t esCLK_MclkReset    ;
    __pSWI_t esCLK_SysInfo      ;
    __pSWI_t esCLK_ModInfo      ;
} SWIHandler_CLK_t;

typedef struct
{
    __pSWI_t esMEM_DramWakeup           ;
    __pSWI_t esMEM_DramSuspend          ;
    __pSWI_t esMEM_RegDramAccess        ;
    __pSWI_t esMEM_UnRegDramAccess      ;
    __pSWI_t esMEM_MasterSet   ;
    __pSWI_t esMEM_MasterGet   ;
    __pSWI_t esMEM_RequestDramUsrMode   ;
    __pSWI_t esMEM_ReleaseDramUsrMode   ;
    __pSWI_t esMEM_BWEnable             ;
    __pSWI_t esMEM_BWDisable            ;
    __pSWI_t esMEM_BWGet                ;
    __pSWI_t esMEM_SramReqBlk           ;
    __pSWI_t esMEM_SramRelBlk           ;
    __pSWI_t esMEM_SramSwitchBlk        ;
} SWIHandler_MEM_t;

typedef struct
{
    __pSWI_t esHID_SendMsg      ;
    __pSWI_t esHID_hiddevreg       ;
    __pSWI_t esHID_hiddevunreg     ;
} SWIHandler_HID_t;

typedef struct
{
    __pSWI_t esPWM_RequestPwmMode   ;
    __pSWI_t esPWM_ReleasePwmMode   ;
    __pSWI_t esPWM_EnterStandby     ;
    __pSWI_t esPWM_UsrEventNotify   ;
    __pSWI_t esPWM_LockCpuFreq      ;
    __pSWI_t esPWM_UnlockCpuFreq    ;
    __pSWI_t esPWM_RegDevice        ;
    __pSWI_t esPWM_UnregDevice      ;
    __pSWI_t esPWM_GetStandbyPara   ;
    __pSWI_t esPWM_SetStandbyMode   ;

} SWIHandler_PWM_t;


typedef struct
{
    __pSWI_t esCHS_Uni2Char       ;
    __pSWI_t esCHS_Char2Uni       ;
    __pSWI_t esCHS_GetChUpperTbl  ;
    __pSWI_t esCHS_GetChLowerTbl  ;

} SWIHandler_CHS_t;

typedef struct
{
    __pSWI_t esMSTUB_RegFuncTbl       ;
    __pSWI_t esMSTUB_UnRegFuncTbl     ;
    __pSWI_t esMSTUB_GetFuncEntry     ;

} SWIHandler_MSTUB_t;

typedef struct
{
    __pSWI_t esCFG_GetKeyValue        ;
    __pSWI_t esCFG_GetSecKeyCount     ;
    __pSWI_t esCFG_GetSecCount        ;
    __pSWI_t esCFG_GetGPIOSecKeyCount ;
    __pSWI_t esCFG_GetGPIOSecData     ;
    __pSWI_t esCFG_Init_Ex               ;
    __pSWI_t esCFG_Exit_Ex               ;
    __pSWI_t esCFG_GetKeyValue_Ex        ;
    __pSWI_t esCFG_GetSecKeyCount_Ex     ;
    __pSWI_t esCFG_GetSecCount_Ex        ;
    __pSWI_t esCFG_GetGPIOSecKeyCount_Ex ;
    __pSWI_t esCFG_GetGPIOSecData_Ex     ;
} SWIHandler_CONFIG_t;

typedef struct
{
    __pSWI_t esINPUT_RegDev      ;
    __pSWI_t esINPUT_UnregDev    ;
    __pSWI_t esINPUT_LdevGrab    ;
    __pSWI_t esINPUT_LdevRelease ;
    __pSWI_t esINPUT_SendEvent   ;
    __pSWI_t esINPUT_LdevFeedback;
    __pSWI_t esINPUT_GetLdevID   ;
    __pSWI_t esINPUT_LdevCtl     ;
} SWIHandler_INPUT_t;

typedef struct
{
    __pSWI_t pthread_attr_destroy           ;
    __pSWI_t pthread_attr_init              ;
    __pSWI_t pthread_attr_setdetachstate    ;
    __pSWI_t pthread_attr_getdetachstate    ;
    __pSWI_t pthread_attr_setschedpolicy    ;
    __pSWI_t pthread_attr_getschedpolicy    ;
    __pSWI_t pthread_attr_setschedparam     ;
    __pSWI_t pthread_attr_getschedparam     ;
    __pSWI_t pthread_attr_setstacksize      ;
    __pSWI_t pthread_attr_getstacksize      ;
    __pSWI_t pthread_attr_setstackaddr      ;
    __pSWI_t pthread_attr_getstackaddr      ;
    __pSWI_t pthread_attr_setstack          ;
    __pSWI_t pthread_attr_getstack          ;
    __pSWI_t pthread_attr_setguardsize      ;
    __pSWI_t pthread_attr_getguardsize      ;
    __pSWI_t pthread_attr_setscope          ;
    __pSWI_t pthread_attr_getscope          ;
    __pSWI_t pthread_system_init            ;
    __pSWI_t pthread_create                 ;
    __pSWI_t pthread_detach                 ;
    __pSWI_t pthread_join                   ;
    __pSWI_t pthread_exit                   ;
    __pSWI_t pthread_once                   ;
    __pSWI_t pthread_cleanup_pop            ;
    __pSWI_t pthread_cleanup_push           ;
    __pSWI_t pthread_cancel                 ;
    __pSWI_t pthread_testcancel             ;
    __pSWI_t pthread_setcancelstate         ;
    __pSWI_t pthread_setcanceltype          ;
    __pSWI_t pthread_atfork                 ;
    __pSWI_t pthread_kill                   ;
    __pSWI_t pthread_self                   ;
    __pSWI_t pthread_mutex_init             ;
    __pSWI_t pthread_mutex_destroy          ;
    __pSWI_t pthread_mutex_lock             ;
    __pSWI_t pthread_mutex_unlock           ;
    __pSWI_t pthread_mutex_trylock          ;
    __pSWI_t pthread_mutexattr_init         ;
    __pSWI_t pthread_mutexattr_destroy      ;
    __pSWI_t pthread_mutexattr_gettype      ;
    __pSWI_t pthread_mutexattr_settype      ;
    __pSWI_t pthread_mutexattr_setpshared   ;
    __pSWI_t pthread_mutexattr_getpshared   ;
    __pSWI_t pthread_condattr_destroy       ;
    __pSWI_t pthread_condattr_init          ;
    __pSWI_t pthread_condattr_getclock      ;
    __pSWI_t pthread_condattr_setclock      ;
    __pSWI_t pthread_cond_init              ;
    __pSWI_t pthread_cond_destroy           ;
    __pSWI_t pthread_cond_broadcast         ;
    __pSWI_t pthread_cond_signal            ;
    __pSWI_t pthread_cond_wait              ;
    __pSWI_t pthread_cond_timedwait         ;
    __pSWI_t pthread_rwlockattr_init        ;
    __pSWI_t pthread_rwlockattr_destroy     ;
    __pSWI_t pthread_rwlockattr_getpshared  ;
    __pSWI_t pthread_rwlockattr_setpshared  ;
    __pSWI_t pthread_rwlock_init            ;
    __pSWI_t pthread_rwlock_destroy         ;
    __pSWI_t pthread_rwlock_rdlock          ;
    __pSWI_t pthread_rwlock_tryrdlock       ;
    __pSWI_t pthread_rwlock_timedrdlock     ;
    __pSWI_t pthread_rwlock_timedwrlock     ;
    __pSWI_t pthread_rwlock_unlock          ;
    __pSWI_t pthread_rwlock_wrlock          ;
    __pSWI_t pthread_rwlock_trywrlock       ;
    __pSWI_t pthread_spin_init              ;
    __pSWI_t pthread_spin_destroy           ;
    __pSWI_t pthread_spin_lock              ;
    __pSWI_t pthread_spin_trylock           ;
    __pSWI_t pthread_spin_unlock            ;
    __pSWI_t pthread_barrierattr_destroy    ;
    __pSWI_t pthread_barrierattr_init       ;
    __pSWI_t pthread_barrierattr_getpshared ;
    __pSWI_t pthread_barrierattr_setpshared ;
    __pSWI_t pthread_barrier_destroy        ;
    __pSWI_t pthread_barrier_init           ;
    __pSWI_t pthread_barrier_wait           ;
    __pSWI_t pthread_getspecific            ;
    __pSWI_t pthread_setspecific            ;
    __pSWI_t pthread_key_create             ;
    __pSWI_t pthread_key_delete             ;
    __pSWI_t sem_close                      ;
    __pSWI_t sem_destroy                    ;
    __pSWI_t sem_getvalue                   ;
    __pSWI_t sem_init                       ;
    __pSWI_t sem_open                       ;
    __pSWI_t sem_post                       ;
    __pSWI_t sem_timedwait                  ;
    __pSWI_t sem_trywait                    ;
    __pSWI_t sem_unlink                     ;
    __pSWI_t sem_wait                       ;
} SWIHandler_PTHREAD_t;


#define SWINO(base,type,isr) (base+(((__u32)(&(((type*)0)->isr)))>>2))

#define SYSCALL_SWINO(x) (SWINO(0, SWIHandler_SWIT_t, x) << 8)

#define SWINO_SIOS  SYSCALL_SWINO(SWIHandler_SIOS)  /* ??׼IOģ??ϵͳ           */
#define SWINO_KRNL  SYSCALL_SWINO(SWIHandler_KRNL)  /* ?ں?ϵͳ                 */
#define SWINO_MEMS  SYSCALL_SWINO(SWIHandler_MEMS)  /* ?ڴ?????ϵͳ             */
#define SWINO_FSYS  SYSCALL_SWINO(SWIHandler_FSYS)  /* ?ļ?ϵͳ                 */
#define SWINO_EXEC  SYSCALL_SWINO(SWIHandler_EXEC)  /* ???̹???ϵͳ             */
#define SWINO_MODS  SYSCALL_SWINO(SWIHandler_MODS)  /* ģ??????ϵͳ             */
#define SWINO_RESM  SYSCALL_SWINO(SWIHandler_RESM)  /* ??Դ????ϵͳ             */
#define SWINO_INT   SYSCALL_SWINO(SWIHandler_INT)   /* ?жϿ???ϵͳ             */
#define SWINO_DMA   SYSCALL_SWINO(SWIHandler_DMA )  /* DMA                      */
#define SWINO_TIME  SYSCALL_SWINO(SWIHandler_TIME)  /* ʱ??????ϵͳ             */
#define SWINO_IPIPE SYSCALL_SWINO(SWIHandler_IPIPE) /*                          */
#define SWINO_PWRS  SYSCALL_SWINO(SWIHandler_PWRS)  /* ???Ĺ???ϵͳ             */
#define SWINO_ERRS  SYSCALL_SWINO(SWIHandler_ERRS)  /* ???�??ʹ???ϵͳ       */
#define SWINO_SVC   SYSCALL_SWINO(SWIHandler_SVC)   /*                          */
#define SWINO_DEV   SYSCALL_SWINO(SWIHandler_DEV)   /* ?豸????ϵͳ             */
#define SWINO_KSRV  SYSCALL_SWINO(SWIHandler_KSRV)  /* ?ں˷???                 */
#define SWINO_PINS  SYSCALL_SWINO(SWIHandler_PINS)  /* pin????                  */
#define SWINO_CLK   SYSCALL_SWINO(SWIHandler_CLK)   /* ʱ?ӷ???                 */
#define SWINO_MEM   SYSCALL_SWINO(SWIHandler_MEM)   /* ?ڴ??豸????             */
#define SWINO_HID   SYSCALL_SWINO(SWIHandler_HID)   /* hid??ϵͳ                */
#define SWINO_PWM   SYSCALL_SWINO(SWIHandler_PWM)   /* pwm??ϵͳ                */
#define SWINO_CHS   SYSCALL_SWINO(SWIHandler_CHS)   /* charset ??ϵͳ           */
#define SWINO_MSTUB SYSCALL_SWINO(SWIHandler_MSTUB) /* module stub ??ϵͳ       */
#define SWINO_INPUT SYSCALL_SWINO(SWIHandler_INPUT) /* ??????ϵͳ               */
#define SWINO_CFG   SYSCALL_SWINO(SWIHandler_CONFIG)/* ??????ϵͳ               */
#define SWINO_PTD   SYSCALL_SWINO(SWIHandler_PTHREAD) /*PTHREADϵͳ*/

// __pSWI_t  const *SWIVectorTable[];
//#define SYSCALL_KRNL(y) SWINO(SWINO_KRNL, SWIHandler_KRNL_t, y)
// yΪϵͳ???ú?????
#define SYSCALL_SIOS(y)  SWINO(SWINO_SIOS, SWIHandler_SIOS_t, y)
#define SYSCALL_KRNL(y)  SWINO(SWINO_KRNL, SWIHandler_KRNL_t, y)
#define SYSCALL_MEMS(y)  SWINO(SWINO_MEMS, SWIHandler_MEMS_t, y)
#define SYSCALL_FSYS(y)  SWINO(SWINO_FSYS, SWIHandler_FSYS_t, y)
#define SYSCALL_EXEC(y)  SWINO(SWINO_EXEC, SWIHandler_EXEC_t, y)
#define SYSCALL_MODS(y)  SWINO(SWINO_MODS, SWIHandler_MODS_t, y)
#define SYSCALL_RESM(y)  SWINO(SWINO_RESM, SWIHandler_RESM_t, y)
#define SYSCALL_INT(y)   SWINO(SWINO_INT,  SWIHandler_INT_t, y)
#define SYSCALL_DMA(y)   SWINO(SWINO_DMA,  SWIHandler_DMA_t, y)
#define SYSCALL_TIME(y)  SWINO(SWINO_TIME, SWIHandler_TIME_t, y)
#define SYSCALL_IPIPE(y) SWINO(SWINO_IPIPE,SWIHandler_IPIPE_t, y)
#define SYSCALL_SVC(y)   SWINO(SWINO_SVC,  SWIHandler_SVC_t, y)
#define SYSCALL_DEV(y)   SWINO(SWINO_DEV,  SWIHandler_DEV_t, y)
#define SYSCALL_KSRV(y)  SWINO(SWINO_KSRV, SWIHandler_KSRV_t, y)
#define SYSCALL_PINS(y)  SWINO(SWINO_PINS, SWIHandler_PINS_t, y)
#define SYSCALL_CLK(y)   SWINO(SWINO_CLK,  SWIHandler_CLK_t, y)
#define SYSCALL_MEM(y)   SWINO(SWINO_MEM,  SWIHandler_MEM_t, y)
#define SYSCALL_HID(y)   SWINO(SWINO_HID,  SWIHandler_HID_t, y)
#define SYSCALL_PWM(y)   SWINO(SWINO_PWM,  SWIHandler_PWM_t, y)
#define SYSCALL_CHS(y)   SWINO(SWINO_CHS,  SWIHandler_CHS_t, y)
#define SYSCALL_MSTUB(y) SWINO(SWINO_MSTUB,SWIHandler_MSTUB_t, y)
#define SYSCALL_INPUT(y) SWINO(SWINO_INPUT,SWIHandler_INPUT_t, y)
#define SYSCALL_CFG(y)   SWINO(SWINO_CFG,  SWIHandler_CONFIG_t, y)
#define SYSCALL_PTD(y)   SWINO(SWINO_PTD,  SWIHandler_PTHREAD_t, y)

#endif
