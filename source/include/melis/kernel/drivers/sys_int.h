#ifndef __SYS_INT_H__
#define __SYS_INT_H__
#include <typedef.h>
#include <kernel/ktype.h>

//define interrupt priority level, the lower value the higher priority
typedef enum __INT_PRIO_LEVEL
{
    INT_PRIO_LEVEL_0 = 0,
    INT_PRIO_LEVEL_1 = 1,
    INT_PRIO_LEVEL_2 = 2,
    INT_PRIO_LEVEL_3 = 3,
    INT_PRIO_LEVEL_
} __int_prio_level_e;

//==============================================================
// define interface for kernel
//==============================================================
__s32 INT_Init(void);
__s32 INT_Exit(void);
__s32 INT_GetCurIrq(void);
__s32 INT_EnterIsr(__s32 irqno);
__s32 INT_EnterFIsr(void);
void  INT_SwitchMask(__s32 irqno);
void  INT_RestoreMask(void);
void  INT_Clear(__u32 irqno);

__s32 esINT_InsISR(__s32 irqno, __pISR_t pIsr, void *pArg);
__s32 esINT_UniISR(__s32 irqno, __pISR_t pIsr);
__s32 esINT_InsFIR(__s32 fiqno, __pISR_t pFirMain, __pCBK_t pFirTail, void *pArg);
__s32 esINT_UniFIR(__s32 fiqno, __pISR_t pFirMain, __pCBK_t pFirTail);
__s32 esINT_SetIRQPrio(__s32 irqno, __s32 prio);
__s32 esINT_DisableINT(__s32 irqno);
__s32 esINT_EnableINT(__s32 irqno);

#endif  //__SYS_INT_H__
