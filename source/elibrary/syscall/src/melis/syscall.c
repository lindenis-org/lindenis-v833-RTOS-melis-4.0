/*
 * =====================================================================================
 *
 *       Filename:  syscall.c
 *
 *    Description:  syscall layer for melis user module(drivers, mod or plugins).
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-06 19:36:51
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  zhijinzeng@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-05-13 18:46:46
 *
 * =====================================================================================
 */

#include <kapi.h>
#include <syscall.h>
#include <mod_defs.h>

__u8 esSIOS_getchar(void)
{
    __u8 result ;
    result = (__u8)__syscall0((long)SYSCALL_SIOS(esSIOS_getchar));
    return result;
}

void esSIOS_gets(char *str)
{
    __syscall1((long)SYSCALL_SIOS(esSIOS_gets), (long)str);
}

void esSIOS_putarg(__u32 arg, char format)
{
    __syscall2((long)SYSCALL_SIOS(esSIOS_putarg), (long)arg, (long)format);
}

void esSIOS_putstr(const char *str)
{
    __syscall1((long)SYSCALL_SIOS(esSIOS_putstr), (long)str);
}

void esSIOS_setbaud(__u32 baud)
{
    __syscall1((long)SYSCALL_SIOS(esSIOS_setbaud), (long)baud);
}

__u32 esKRNL_TCreate(void (*entry)(void *p_arg), void *p_arg, __u32 stksize, __u16 id_priolevel)
{
    __u32 result ;
    result = (__u32) __syscall4((long)SYSCALL_KRNL(esKRNL_TCreate), (long)entry, (long)p_arg, (long)stksize, (long)id_priolevel);
    return result;
}

__s8 esKRNL_TDel(__u32 prio)
{
    __s8 result ;
    result = (__s8)__syscall1((long)SYSCALL_KRNL(esKRNL_TDel), (long)prio);
    return result;
}

__s8 esKRNL_TDelReq(__u32 prio)
{
    __s8 result ;
    result = (__s8)__syscall1((long)SYSCALL_KRNL(esKRNL_TDelReq), (long)prio);
    return result;
}

__s8 esKRNL_TaskDel(__u32 prio)
{
    __s8 result ;
    result = (__s8)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskDel), (long)prio);
    return result;
}

__s8 esKRNL_TaskDelReq(__u32 prio_ex)
{
    __s8 result ;
    result = (__s8)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskDelReq), (long)prio_ex);
    return result;
}

__u8 esKRNL_TaskQuery(__u32 prio, __krnl_tcb_t *p_task_data)
{
    __u8 result ;
    result = (__u8)__syscall2((long)SYSCALL_KRNL(esKRNL_TaskQuery), (long)prio, (long)p_task_data);
    return result;
}

__u8 esKRNL_TaskResume(__u32 prio)
{
    __u8 result ;
    result = (__s8)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskResume), (long)prio);
    return result;
}

__u8 esKRNL_TaskSuspend(__u32 prio)
{
    __u8 result ;
    result = (__s8)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskSuspend), (long)prio);
    return result;
}

void esKRNL_TimeDly(__u16 ticks)
{
    __s8 result ;
    result = __syscall1((long)SYSCALL_KRNL(esKRNL_TimeDly), (long)ticks);
    return;
}

__u8 esKRNL_TimeDlyResume(__u32 prio_ex)
{
    __u8 result ;
    result = (__u8)__syscall1((long)SYSCALL_KRNL(esKRNL_TimeDlyResume), (long)prio_ex);
    return result;
}

__u32 esKRNL_TimeGet(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KRNL(esKRNL_TimeGet));
    return result;
}

void esKRNL_TimeSet(__u32 ticks)
{
    __u32 result ;
    result = __syscall1((long)SYSCALL_KRNL(esKRNL_TimeSet), (long)ticks);
}

__u16 esKRNL_SemAccept(void *psem)
{
    __u16 result ;
    result = (__u16)__syscall1((long)SYSCALL_KRNL(esKRNL_SemAccept), (long)psem);
    return result;
}

__hdle esKRNL_SemCreate(__u16 cnt)
{
    __hdle result ;
    result = (__hdle)(__syscall1((long)SYSCALL_KRNL(esKRNL_SemCreate), (long)cnt));
    return result;
}

__hdle esKRNL_SemDel(__hdle sem, __u8 opt, __u8 *err)
{
    __hdle result ;
    result = (__hdle)__syscall3((long)SYSCALL_KRNL(esKRNL_SemDel), (long)sem, (long)opt, (long)err);
    return result;
}

void esKRNL_SemPend(__hdle sem, __u16 timeout, __u8 *err)
{
    __s32 result ;
    result = __syscall3((long)SYSCALL_KRNL(esKRNL_SemPend), (long)sem, (long)timeout, (long)err);
    return;
}

__u8 esKRNL_SemPost(__hdle sem)
{
    __u8 result ;
    result = (__u8)__syscall1((long)SYSCALL_KRNL(esKRNL_SemPost), (long)sem);
    return result;
}

__u8 esKRNL_SemQuery(__hdle sem, OS_SEM_DATA *p_sem_data)
{
    __u8 result ;
    result = (__u8)__syscall2((long)SYSCALL_KRNL(esKRNL_SemQuery), (long)sem, (long)p_sem_data);
    return result;
}

void esKRNL_SemSet(__hdle pevent, __u16 cnt, __u8 *err)
{
    __u8 result ;
    result = __syscall3((long)SYSCALL_KRNL(esKRNL_SemSet), (long)pevent, (long)cnt, (long)err);
    return;
}

__u32 esKRNL_MboxAccept(__hdle pevent)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_KRNL(esKRNL_MboxAccept), (long)pevent);
    return result;
}

__hdle esKRNL_MboxCreate(__u32 msg)
{
    __hdle result ;
    result = (__hdle)__syscall1((long)SYSCALL_KRNL(esKRNL_MboxCreate), (long)msg);
    return result;
}

__u32 esKRNL_MboxDel(__hdle pevent, __u8 opt, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall3((long)SYSCALL_KRNL(esKRNL_MboxDel), (long)pevent, (long)opt, (long)err);
    return result;
}

__u32 esKRNL_MboxPend(__hdle pevent, __u16 timeout, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall3((long)SYSCALL_KRNL(esKRNL_MboxPend), (long)pevent, (long)timeout, (long)err);
    return result;
}

__u8 esKRNL_MboxPost(__hdle pevent, __u32 msg)
{
    __u8 result ;
    result = (__u8)__syscall2((long)SYSCALL_KRNL(esKRNL_MboxPost), (long)pevent, (long)msg);
    return result;
}

__u8 esKRNL_MboxPostOpt(__hdle pevent, __u32 msg, __u8 opt)
{
    __u8 result ;
    result = (__u8)__syscall3((long)SYSCALL_KRNL(esKRNL_MboxPostOpt), (long)pevent, (long)msg, (long)opt);
    return result;
}

__u8 esKRNL_MboxQuery(__hdle pevent, void *p_mbox_data)
{
    __u8 result ;
    result = (__u8)__syscall2((long)SYSCALL_KRNL(esKRNL_MboxQuery), (long)pevent, (long)p_mbox_data);
    return result;
}

__hdle esKRNL_MutexCreate(__u8 prio, __u8 *err)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_KRNL(esKRNL_MutexCreate), (long)prio, (long)err);
    return result;
}

__u32 esKRNL_MutexDel(__hdle pevent, __u8 opt, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall3((long)SYSCALL_KRNL(esKRNL_MutexDel), (long)pevent, (long)opt, (long)err);
    return result;
}

void esKRNL_MutexPend(__hdle pevent, __u16 timeout, __u8 *err)
{
    __u32 result ;
    result = __syscall3((long)SYSCALL_KRNL(esKRNL_MutexPend), (long)pevent, (long)timeout, (long)err);
}

__u8 esKRNL_MutexPost(__hdle pevent)
{
    __u8 result ;
    result = (__u8)__syscall1((long)SYSCALL_KRNL(esKRNL_MutexPost), (long)pevent);
    return result;
}

__u32 esKRNL_QAccept(__hdle pevent, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall2((long)SYSCALL_KRNL(esKRNL_QAccept), (long)pevent, (long)err);
    return result;
}

__hdle esKRNL_QCreate(__u16 size)
{
    __hdle result ;
    result = (__hdle)__syscall1((long)SYSCALL_KRNL(esKRNL_QCreate), (long)size);
    return result;
}

__hdle esKRNL_QDel(__hdle pevent, __u8 opt, __u8 *err)
{
    __hdle result ;
    result = (__hdle)__syscall3((long)SYSCALL_KRNL(esKRNL_QDel), (long)pevent, (long)opt, (long)err);
    return result;
}

__u8 esKRNL_QFlush(__hdle pevent)
{
    __u8 result ;
    result = (__u8)__syscall1((long)SYSCALL_KRNL(esKRNL_QFlush), (long)pevent);
    return result;
}

__u32 esKRNL_QPend(__hdle pevent, __u16 timeout, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall3((long)SYSCALL_KRNL(esKRNL_QPend), (long)pevent, (long)timeout, (long)err);
    return result;
}

__u8 esKRNL_QPost(__hdle pevent, __u32 msg)
{
    __u8 result ;
    result = (__u8)__syscall2((long)SYSCALL_KRNL(esKRNL_QPost), (long)pevent, (long)msg);
    return result;
}

__u8 esKRNL_QPostFront(__hdle pevent, void *msg)
{
    __u8 result ;
    result = (__u8)__syscall2((long)SYSCALL_KRNL(esKRNL_QPostFront), (long)pevent, (long)msg);
    return result;
}

__u8 esKRNL_QPostOpt(__hdle pevent, void *msg, __u8 opt)
{
    __u8 result ;
    result = (__u8)__syscall3((long)SYSCALL_KRNL(esKRNL_QPostOpt), (long)pevent, (long)msg, (long)opt);
    return result;
}

__u8 esKRNL_QQuery(__hdle pevent, __krnl_q_data_t *p_q_data)
{
    __u8 result ;
    result = (__u8)__syscall2((long)SYSCALL_KRNL(esKRNL_QQuery), (long)pevent, (long)p_q_data);
    return result;
}

__u32 esKRNL_FlagAccept(__hdle pgrp, __u32 flags, __u8 wait_type, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_KRNL(esKRNL_FlagAccept), (long)pgrp, (long)flags, (long)wait_type, (long)err);
    return result;
}

__hdle esKRNL_FlagCreate(__u32 flags, __u8 *err)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_KRNL(esKRNL_FlagCreate), (long)flags, (long)err);
    return result;
}

__u32 esKRNL_FlagDel(__hdle pgrp, __u8 opt, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall3((long)SYSCALL_KRNL(esKRNL_FlagDel), (long)pgrp, (long)opt, (long)err);
    return result;
}

__u8 esKRNL_FlagNameGet(__hdle pgrp, __u8 *pname, __u8 *err)
{
    __u8 result ;
    result = (__u8)__syscall3((long)SYSCALL_KRNL(esKRNL_FlagNameGet), (long)pgrp, (long)pname, (long)err);
    return result;
}

void esKRNL_FlagNameSet(__hdle pgrp, __u8 *pname, __u8 *err)
{
    __u8 result ;
    result = __syscall3((long)SYSCALL_KRNL(esKRNL_FlagNameSet), (long)pgrp, (long)pname, (long)err);
}

__u32 esKRNL_FlagPend(__hdle pgrp, __u32 flags, __u32 waittype_timeout, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_KRNL(esKRNL_FlagPend), (long)pgrp, (long)flags, (long)waittype_timeout, (long)err);
    return result;
}

__u32 esKRNL_FlagPendGetFlagsRdy(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KRNL(esKRNL_FlagPendGetFlagsRdy));
    return result;
}

__u32 esKRNL_FlagPost(__hdle pgrp, __u32 flags, __u8 opt, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_KRNL(esKRNL_FlagPost), (long)pgrp, (long)flags, (long)opt, (long)err);
    return result;
}

__u32 esKRNL_FlagQuery(__hdle pgrp, __u8 *err)
{
    __u32 result ;
    result = (__u32)__syscall2((long)SYSCALL_KRNL(esKRNL_FlagQuery), (long)pgrp, (long)err);
    return result;
}

__hdle esKRNL_TmrCreate(__u32 period, __u8 opt, OS_TMR_CALLBACK callback, void *callback_arg)
{
    __hdle result ;
    result = (__hdle)__syscall4((long)SYSCALL_KRNL(esKRNL_TmrCreate), (long)period, (long)opt, (long)callback, (long)callback_arg);
    return result;
}

__s32 esKRNL_TmrDel(__hdle ptmr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrDel), (long)ptmr);
    return result;
}

__s32 esKRNL_TmrStart(__hdle ptmr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrStart), (long)ptmr);
    return result;
}

__s32 esKRNL_TmrStop(__hdle ptmr, __s8 opt, void *callback_arg)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_KRNL(esKRNL_TmrStop), (long)ptmr, (long)opt, (long)callback_arg);
    return result;
}

__u32 esKRNL_TmrRemainGet(__hdle ptmr)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrRemainGet), (long)ptmr);
    return result;
}

__u8 esKRNL_TmrStateGet(__hdle  ptmr)
{
    __u8 result ;
    result = (__u8)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrStateGet), (long)ptmr);
    return result;
}

__s16 esKRNL_TmrError(__hdle ptmr)
{
    __s16 result ;
    result = (__s16)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrError), (long)ptmr);
    return result;
}

__u16 esKRNL_Version(void)
{
    __u16 result ;
    result = (__u16)__syscall0((long)SYSCALL_KRNL(esKRNL_Version));
    return result;
}

void esKRNL_SchedLock(void)
{
    __u16 result ;
    result = __syscall0((long)SYSCALL_KRNL(esKRNL_SchedLock));
}

void esKRNL_SchedUnlock(void)
{
    __u16 result ;
    result = __syscall0((long)SYSCALL_KRNL(esKRNL_SchedUnlock));
}

__pCBK_t esKRNL_GetCallBack(__pCBK_t cb)
{
    __pCBK_t result ;
    result = (__pCBK_t)__syscall1((long)SYSCALL_KRNL(esKRNL_GetCallBack), (long)cb);
    return result;
}

__s32 esKRNL_CallBack(__pCBK_t cb, void *arg)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_KRNL(esKRNL_CallBack), (long)cb, (long)arg);
    return result;
}

__u32 esKRNL_GetTIDCur(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KRNL(esKRNL_GetTIDCur));
    return result;
}

__hdle esKRNL_SktCreate(__u32 depth, __u32 dbuf_attr, __u32 mbuf_attr)
{
    __hdle result ;
    result = (__hdle)__syscall3((long)SYSCALL_KRNL(esKRNL_SktCreate), (long)depth, (long)dbuf_attr, (long)mbuf_attr);
    return result;
}

__s32 esKRNL_SktDel(__hdle skt, __u8 opt)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_KRNL(esKRNL_SktDel), (long)skt, (long)opt);
    return result;
}

__krnl_sktfrm_t *esKRNL_SktPend(__hdle skt, __u8 user, __u32 timeout)
{
    __krnl_sktfrm_t *result ;
    result = (__krnl_sktfrm_t *)__syscall3((long)SYSCALL_KRNL(esKRNL_SktPend), (long)skt, (long)user, (long)timeout);
    return result;
}

__s32 esKRNL_SktPost(__hdle skt, __u8 user, __krnl_sktfrm_t *frm)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_KRNL(esKRNL_SktPost), (long)skt, (long)user, (long)frm);
    return result;
}

__krnl_sktfrm_t *esKRNL_SktAccept(__hdle skt, __u8 user)
{
    __krnl_sktfrm_t *result ;
    result = (__krnl_sktfrm_t *)__syscall2((long)SYSCALL_KRNL(esKRNL_SktAccept), (long)skt, (long)user);
    return result;
}

__u32 esKRNL_Time(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KRNL(esKRNL_Time));
    return result;
}

void esKRNL_TaskPrefEn(__u32 en)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskPrefEn), (long)en);
}

void esKRNL_MemLeakChk(__u32 en)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KRNL(esKRNL_MemLeakChk), (long)en);
}

void esKRNL_DumpStack(void)
{
    __u32 result;
    result = (__u32)__syscall0((long)SYSCALL_KRNL(esKRNL_DumpStack));
}

void *esMEMS_Palloc(__u32 npage, __u32 mode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_MEMS(esMEMS_Palloc), (long)npage, (long)mode);
    return result;
}

void esMEMS_Pfree(void *mblk, __u32 npage)
{
    __u32 result ;
    result = (__u32)__syscall2((long)SYSCALL_MEMS(esMEMS_Pfree), (long)mblk, (long)npage);
}

__u32 esMEMS_VA2PA(__u32 vaddr)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_MEMS(esMEMS_VA2PA), (long)vaddr);
    return result;
}

__s32 esMEMS_VMCreate(void *pBlk, __u32 npage, __s8 domain)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_MEMS(esMEMS_VMCreate), (long)pBlk, (long)npage, (long)domain);
    return result;
}

void esMEMS_VMDelete(void *pBlk, __u32 npage)
{
    __u32 result ;
    result = (__u32)__syscall2((long)SYSCALL_MEMS(esMEMS_VMDelete), (long)pBlk, (long)npage);
}

__s32 esMEMS_HeapCreate(void *heapaddr, __u32 initnpage)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_MEMS(esMEMS_HeapCreate), (long)heapaddr, (long)initnpage);
    return result;
}

void esMEMS_HeapDel(void *heap)
{
    __s32 result ;
    result = __syscall1((long)SYSCALL_MEMS(esMEMS_HeapDel), (long)heap);
}

void *esMEMS_Malloc(void *heap, __u32 size)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_MEMS(esMEMS_Malloc), (long)heap, (long)size);
    return result;
}

void esMEMS_Mfree(void *heap, void *ptr)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_MEMS(esMEMS_Mfree), (long)heap, (long)ptr);
}

void *esMEMS_Realloc(void *heap, void *f, __u32 size)
{
    void *result ;
    result = (void *)__syscall3((long)SYSCALL_MEMS(esMEMS_Realloc), (long)heap, (long)f, (long)size);
    return result;
}

void *esMEMS_Calloc(void *heap, __u32 n, __u32 m)
{
    void *result ;
    result = (void *)__syscall3((long)SYSCALL_MEMS(esMEMS_Calloc), (long)heap, (long)n, (long)m);
    return result;
}

void *esMEMS_Balloc(__u32 size)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_MEMS(esMEMS_Balloc), (long)size);
    return result;
}

void esMEMS_Bfree(void *addr, __u32 size)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_MEMS(esMEMS_Bfree), (long)addr, (long)size);
}

__s32 esMEMS_Info(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_MEMS(esMEMS_Info));
    return result;
}

void esMEMS_CleanDCache(void)
{
    __u32 result;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_CleanDCache));
}

void esMEMS_CleanFlushDCache(void)
{
    __u32 result;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_CleanFlushDCache));
}

void esMEMS_FlushDCache(void)
{
    __u32 result;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_FlushDCache));
}

void esMEMS_CleanFlushCache(void)
{
    __u32 result;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_CleanFlushCache));
}

void esMEMS_FlushICache(void)
{
    __u32 result;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_FlushICache));
}

void esMEMS_FlushCache(void)
{
    __u32 result;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_FlushCache));
}

__u32 esMEMS_GetIoVAByPA(__u32 phyaddr, __u32 size)
{
    __u32 result;
    result = (__u32)__syscall1((long)SYSCALL_MEMS(esMEMS_GetIoVAByPA), (long)phyaddr);
    return result;
}

void esMEMS_CleanDCacheRegion(void *adr, __u32 bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_CleanDCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_CleanFlushDCacheRegion(void *adr, __u32 bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_CleanFlushDCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_CleanFlushCacheRegion(void *adr, __u32 bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_CleanFlushCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_FlushDCacheRegion(void *adr, __u32 bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_FlushDCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_FlushICacheRegion(void *adr, __u32 bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_FlushICacheRegion), (long)adr, (long)bytes);
}

void esMEMS_FlushCacheRegion(void *adr, __u32 bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_FlushCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_CleanInvalidateCacheAll(void)
{
    __syscall0((long)SYSCALL_MEMS(esMEMS_CleanInvalidateCacheAll));
}

__u32 esMEMS_FreeMemSize(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_FreeMemSize));
    return result;
}

__u32 esMEMS_TotalMemSize(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_TotalMemSize));
    return result;
}

void *esMEMS_VMalloc(__u32 size)
{
    void *result;
    result = (void *)__syscall0((long)SYSCALL_MEMS(esMEMS_VMalloc));
    return result;
}

void esMEMS_VMfree(void *ptr)
{
    __u32 result;
    result = (__u32)__syscall0((long)SYSCALL_MEMS(esMEMS_VMfree));
}

__hdle esFSYS_opendir(const char *pDirName)
{
    __hdle result ;
    result = (__hdle)__syscall1((long)SYSCALL_FSYS(esFSYS_opendir), (long)pDirName);
    return result;
}

__s32 esFSYS_closedir(__hdle hDir)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_closedir), (long)hDir);
    return result;
}

__hdle esFSYS_readdir(__hdle hDir)
{
    __hdle result ;
    result = (__hdle)__syscall1((long)SYSCALL_FSYS(esFSYS_readdir), (long)hDir);
    return result;
}

void esFSYS_rewinddir(__hdle hDir)
{
    __hdle result ;
    result = (__hdle)__syscall1((long)SYSCALL_FSYS(esFSYS_rewinddir), (long)hDir);
}

__s32 esFSYS_mkdir(const char *pDirName)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_mkdir), (long)pDirName);
    return result;
}

__s32 esFSYS_rmdir(const char *pDirName)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_rmdir), (long)pDirName);
    return result;
}

__s32 esFSYS_remove(const char *pFileName)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_remove), (long)pFileName);
    return result;
}

__s32 esFSYS_rename(const char *newname, const char *oldname)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_rename), (long)newname, (long)oldname);
    return result;
}

__hdle esFSYS_fopen(const char *pFileName, const char *pMode)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_FSYS(esFSYS_fopen), (long)pFileName, (long)pMode);
    return result;
}

__s32 esFSYS_fclose(__hdle hFile)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_fclose), (long)hFile);
    return result;
}

__u32 esFSYS_fread(void *pData, __u32 Size, __u32 N, __hdle hFile)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_FSYS(esFSYS_fread), (long)pData, (long)Size, (long)N, (long)hFile);
    return result;
}

__u32 esFSYS_fwrite(const void *pData, __u32 Size, __u32 N, __hdle hFile)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_FSYS(esFSYS_fwrite), (long)pData, (long)Size, (long)N, (long)hFile);
    return result;
}

__s32 esFSYS_fseek(__hdle hFile, __s32 Offset, __s32 Whence)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_FSYS(esFSYS_fseek), (long)hFile, (long)Offset, (long)Whence);
    return result;
}

__s32 esFSYS_ftell(__hdle hFile)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_ftell), (long)hFile);
    return result;
}

__s32 esFSYS_fseekex(__hdle hFile, __s32 l_off, __s32 h_off, __s32 Whence)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_FSYS(esFSYS_fseekex), (long)hFile, (long)l_off, (long)h_off, (long)Whence);
    return result;
}

__s32 esFSYS_ftellex(__hdle hFile, __s32 *l_pos, __s32 *h_pos)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_FSYS(esFSYS_ftellex), (long)hFile, (long)l_pos, (long)h_pos);
    return result;
}

__s32 esFSYS_fioctrl(__hdle hFile, __s32 Cmd, __s32 Aux, void *pBuffer)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_FSYS(esFSYS_fioctrl), (long)hFile, (long)Cmd, (long)Aux, (long)pBuffer);
    return result;
}

__s32 esFSYS_ferror(__hdle hFile)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_ferror), (long)hFile);
    return result;
}

void esFSYS_ferrclr(__hdle hFile)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_FSYS(esFSYS_ferrclr), (long)hFile);
    // return result;
}

__s32 esFSYS_premove(const char *pFileName)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_premove), (long)pFileName);
    return result;
}

__s32 esFSYS_prename(const char *newfilename, const char *oldfilename)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_prename), (long)newfilename, (long)oldfilename);
    return result;
}

__hdle esFSYS_popen(const char *PartName, const char *pMode)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_FSYS(esFSYS_popen), (long)PartName, (long)pMode);
    return result;
}

__s32 esFSYS_pclose(__hdle hPart)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_pclose), (long)hPart);
    return result;
}

__u32 esFSYS_pread(void *pData, __u32 sector, __u32 n, __hdle hPart)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_FSYS(esFSYS_pread), (long)pData, (long)sector, (long)n, (long)hPart);
    return result;
}

__u32 esFSYS_pwrite(const void *pData, __u32 Sector, __u32 N, __hdle hPart)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_FSYS(esFSYS_pwrite), (long)pData, (long)Sector, (long)N, (long)hPart);
    return result;
}

__s32 esFSYS_pioctrl(__hdle hPart, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_FSYS(esFSYS_pioctrl), (long)hPart, (long)cmd, (long)aux, (long)pbuffer);
    return result;
}

__s32 esFSYS_perror(__hdle hPart)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_perror), (long)hPart);
    return result;
}

void esFSYS_perrclr(__hdle hPart)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_FSYS(esFSYS_perrclr), (long)hPart);
    //return result;
}

__s32 esFSYS_mntparts(__hdle hNode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_mntparts), (long)hNode);
    return result;
}

__s32 esFSYS_umntparts(__hdle hNode, __u32 force)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_umntparts), (long)hNode, (long)force);
    return result;
}

__s32 esFSYS_fsreg(__hdle hFS)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_fsreg), (long)hFS);
    return result;
}

__s32 esFSYS_fsunreg(__hdle hFS)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_fsunreg), (long)hFS);
    return result;
}

__s32 esFSYS_pdreg(__hdle hPD)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_pdreg), (long)hPD);
    return result;
}

__s32 esFSYS_pdunreg(__hdle hPD)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_pdunreg), (long)hPD);
    return result;
}

__s32 esFSYS_partfslck(char *partname)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_partfslck), (long)partname);
    return result;
}

__s32 esFSYS_partfsunlck(char *partname)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_partfsunlck), (long)partname);
    return result;
}

__s32 esFSYS_mntfs(__hdle part)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_mntfs), (long)part);
    return result;
}

__s32 esFSYS_umntfs(__hdle part, __s32 force)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_umntfs), (long)part, (long)force);
    return result;
}

__s32 esFSYS_format(const char *partname, const char *fstype, __hdle fmtpara)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_FSYS(esFSYS_format), (long)partname, (long)fstype, (long)fmtpara);
    return result;
}

__s32 esFSYS_statfs(const char *path, __hdle buf, __u32 flags)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_FSYS(esFSYS_statfs), (long)path, (long)buf, (long)flags);
    return result;
}

__s32 esFSYS_fsdbg(const char *cmd, const char *para)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_fsdbg), (long)cmd, (long)para);
    return result;
}

__s32 esFSYS_fsync(__hdle hFile)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_fsync), (long)hFile);
    return result;
}

__s32 esFSYS_fstat(__hdle hFile, void *stat_buf)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_fstat), (long)hFile, (long)stat_buf);
    return result;
}

__s32 esFSYS_statpt(const char *path, __hdle buf)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_statpt), (long)path, (long)buf);
    return result;
}

__s32 esFSYS_setfs(char *partname, __u32 cmd, __s32 aux, char *para)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_FSYS(esFSYS_setfs), (long)partname, (long)cmd, (long)aux, (long)para);
    return result;
}

__s32 esFSYS_getfscharset(const char *partname, __s32 *pCharset)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_getfscharset), (long)partname, (long)pCharset);
    return result;
}

__s32 esFSYS_file2fd(__hdle hFile)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_file2fd), (long)hFile);
    return result;
}

__hdle esFSYS_fd2file(__s32 fd)
{
    __hdle result ;
    result = (__hdle)__syscall1((long)SYSCALL_FSYS(esFSYS_fd2file), (long)fd);
    return result;
}

__s32 esFSYS_open(const char *name, __s32 flag, __s32 prems)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_FSYS(esFSYS_open), (long)name, (long)flag, (long)prems);
    return result;
}

__s32 esFSYS_querypartupdateflag(const char *path, __bool *flag)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_querypartupdateflag), (long)path, (long)flag);
    return result;
}

__s32 esFSYS_clearpartupdateflag(const char *path)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_FSYS(esFSYS_clearpartupdateflag), (long)path);
    return result;
}

__s32 esFSYS_ftruncate(__hdle  filehandle, __u32 length)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_FSYS(esFSYS_ftruncate), (long)filehandle, (long)length);
    return result;
}

// EXEC
__u8 esEXEC_PCreate(const char *filename, void *p_arg, __u32 mode, __u32 *ret)
{
    __u8 result ;
    result = (__u8)__syscall4((long)SYSCALL_EXEC(esEXEC_PCreate), (long)filename, (long)p_arg, (long)mode, (long)ret);
    return result;
}

__s8 esEXEC_PDel(__u8 id)
{
    __s8 result ;
    result = (__s8)__syscall1((long)SYSCALL_EXEC(esEXEC_PDel), (long)id);
    return result;
}

__s8 esEXEC_PDelReq(__u8 pid)
{
    __s8 result ;
    result = (__s8)__syscall1((long)SYSCALL_EXEC(esEXEC_PDelReq), (long)pid);
    return result;
}

__s8 esEXEC_Run(const char *pfilename, void *p_arg, __u32 mode, __u32 *ret)
{
    __s8 result ;
    result = (__s8)__syscall4((long)SYSCALL_EXEC(esEXEC_Run), (long)pfilename, (long)p_arg, (long)mode, (long)ret);
    return result;
}

// MODS
__u32 esMODS_MInstall(const char *mfile, __u8 mode)
{
    __u32 result ;
    result = (__u32)__syscall2((long)SYSCALL_MODS(esMODS_MInstall), (long)mfile, (long)mode);
    return result;
}

__s32 esMODS_MUninstall(__u8 id)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_MODS(esMODS_MUninstall), (long)id);
    return result;
}

__mp *esMODS_MOpen(__u8 id, __u8 mode)
{
    __mp *result ;
    result = (__mp *)__syscall2((long)SYSCALL_MODS(esMODS_MOpen), (long)id, (long)mode);
    return result;
}

__s32 esMODS_MClose(__mp *mp)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_MODS(esMODS_MClose), (long)mp);
    return result;
}

__u32 esMODS_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_MODS(esMODS_MRead), (long)pdata, (long)size, (long)n, (long)mp);
    return result;
}

__u32 esMODS_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_MODS(esMODS_MWrite), (long)pdata, (long)size, (long)n, (long)mp);
    return result;
}

__s32 esMODS_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_MODS(esMODS_MIoctrl), (long)mp, (long)cmd, (long)aux, (long)pbuffer);
    return result;
}

// RESM
void *esRESM_ROpen(const char *file, const char *mode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_RESM(esRESM_ROpen), (long)file, (long)mode);
    return result;
}

__s32 esRESM_RClose(__resm_rsb_t *res)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_RESM(esRESM_RClose), (long)res);
    return result;
}

__u32 esRESM_RRead(void *pdata, __u32 size, __u32 n, __resm_rsb_t *res)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_RESM(esRESM_RRead), (long)pdata, (long)size, (long)n, (long)res);
    return result;
}

__s32 esRESM_RSeek(__resm_rsb_t *res, __s32 Offset, __s32 Whence)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_RESM(esRESM_RSeek), (long)res, (long)Offset, (long)Whence);
    return result;
}

// INT
__s32 esINT_InsISR(__s32 irqno, __pISR_t pIsr, void *pArg)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_INT(esINT_InsISR), (long)irqno, (long)pIsr, (long)pArg);
    return result;
}

__s32 esINT_UniISR(__s32 irqno, __pISR_t pIsr)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_INT(esINT_UniISR), (long)irqno, (long)pIsr);
    return result;
}

__s32 esINT_InsFIR(__s32 fiqno, __pISR_t pFirMain, __pCBK_t pFirTail, void *pArg)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_INT(esINT_InsFIR), (long)fiqno, (long)pFirMain, (long)pFirTail, (long)pArg);
    return result;
}

__s32 esINT_UniFIR(__s32 fiqno, __pISR_t pFirMain, __pCBK_t pFirTail)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_INT(esINT_UniFIR), (long)fiqno, (long)pFirMain, (long)pFirTail);
    return result;
}

__s32 esINT_SetIRQPrio(__s32 irqno, __s32 prio)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_INT(esINT_SetIRQPrio), (long)irqno, (long)prio);
    return result;
}

__s32 esINT_DisableINT(__s32 irqno)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_INT(esINT_DisableINT), (long)irqno);
    return result;
}

__s32 esINT_EnableINT(__s32 irqno)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_INT(esINT_EnableINT), (long)irqno);
    return result;
}

// DMA
__hdle esDMA_Request(char *user, __u32 type)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_DMA(esDMA_Request), (long)user, (long)type);
    return result;
}

__s32 esDMA_Release(__hdle dma)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_DMA(esDMA_Release), (long)dma);
    return result;
}

__s32 esDMA_Setting(__hdle dma, void *arg)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_DMA(esDMA_Setting), (long)dma, (long)arg);
    return result;
}

__s32 esDMA_Start(__hdle dma, __u32 saddr, __u32 daddr, __u32 len)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_DMA(esDMA_Start), (long)dma, (long)saddr, (long)daddr, (long)len);
    return result;
}

__s32 esDMA_Stop(__hdle dma)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_DMA(esDMA_Stop), (long)dma);
    return result;
}

__s32 esDMA_Restart(__hdle dma)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_DMA(esDMA_Restart), (long)dma);
    return result;
}

__u32 esDMA_QueryStat(__hdle dma)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_DMA(esDMA_QueryStat), (long)dma);
    return result;
}

__u32 esDMA_QuerySrc(__hdle dma)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_DMA(esDMA_QuerySrc), (long)dma);
    return result;
}

__u32 esDMA_QueryDst(__hdle dma)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_DMA(esDMA_QueryDst), (long)dma);
    return result;
}

__s32 esDMA_EnableINT(__hdle dma, __s32 type)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_DMA(esDMA_EnableINT), (long)dma, (long)type);
    return result;
}

__s32 esDMA_DisableINT(__hdle dma, __s32 type)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_DMA(esDMA_DisableINT), (long)dma, (long)type);
    return result;
}

__u32 esDMA_QueryRestCount(__hdle dma)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_DMA(esDMA_QueryRestCount), (long)dma);
    return result;
}

__s32 esDMA_ChangeMode(__hdle dma, __s32 mode)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_DMA(esDMA_ChangeMode), (long)dma, (long)mode);
    return result;
}

__s32 esDMA_RegDmaHdler(__hdle dma, __s32 type, __pCBK_t hdler, void *arg)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_DMA(esDMA_RegDmaHdler), (long)dma, (long)type, (long)hdler, (long)arg);
    return result;
}

__s32 esDMA_UnregDmaHdler(__hdle dma, __s32 type, __pCBK_t hdler)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_DMA(esDMA_UnregDmaHdler), (long)dma, (long)type, (long)hdler);
    return result;
}

void esDMA_Information(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_DMA(esDMA_Information));
    // return result;
}

__hdle esTIME_RequestTimer(__hw_tmr_type_t *pType, __pCBK_t pHdlr, void *pArg, char *pUsr)
{
    __hdle result ;
    result = (__hdle)__syscall4((long)SYSCALL_TIME(esTIME_RequestTimer), (long)pType, (long)pHdlr, (long)pArg, (long)pUsr);
    return result;
}

__s32 esTIME_ReleaseTimer(__hdle hTmr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_ReleaseTimer), (long)hTmr);
    return result;
}

__s32 esTIME_StartTimer(__hdle hTmr, __u32 nPeriod, __hw_tmr_mode_e nMode)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_TIME(esTIME_StartTimer), (long)hTmr, (long)nPeriod, (long)nMode);
    return result;
}

__s32 esTIME_StopTimer(__hdle hTmr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_StopTimer), (long)hTmr);
    return result;
}

__u32 esTIME_QuerryTimer(__hdle hTmr)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_TIME(esTIME_QuerryTimer), (long)hTmr);
    return result;
}

__s32 esTIME_GetTime(__awos_time_t *time)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_GetTime), (long)time);
    return result;
}

__s32 esTIME_SetTime(__awos_time_t *time)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_SetTime), (long)time);
    return result;
}

__s32 esTIME_GetDate(__awos_date_t *date)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_GetDate), (long)date);
    return result;
}

__s32 esTIME_SetDate(__awos_date_t *date)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_SetDate), (long)date);
    return result;
}

__hdle esTIME_RequestAlarm(__u32 mode)
{
    __hdle result ;
    result = (__hdle)__syscall1((long)SYSCALL_TIME(esTIME_RequestAlarm), (long)mode);
    return result;
}

__s32 esTIME_ReleaseAlarm(__hdle alarm)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_ReleaseAlarm), (long)alarm);
    return result;
}

__s32 esTIME_StartAlarm(__hdle alarm, __u32 time)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_TIME(esTIME_StartAlarm), (long)alarm, (long)time);
    return result;
}

__s32 esTIME_StopAlarm(__hdle alarm)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_StopAlarm), (long)alarm);
    return result;
}

__u32 esTIME_QuerryAlarm(__hdle alarm)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_TIME(esTIME_QuerryAlarm), (long)alarm);
    return result;
}

__hdle esTIME_RequestCntr(__pCB_ClkCtl_t cb, char *pUsr)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_TIME(esTIME_RequestCntr), (long)cb, (long)pUsr);
    return result;
}

__s32 esTIME_ReleaseCntr(__hdle hCntr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_ReleaseCntr), (long)hCntr);
    return result;
}

__s32 esTIME_StartCntr(__hdle hCntr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_StartCntr), (long)hCntr);
    return result;
}

__s32 esTIME_StopCntr(__hdle hCntr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_StopCntr), (long)hCntr);
    return result;
}

__s32 esTIME_PauseCntr(__hdle hCntr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_PauseCntr), (long)hCntr);
    return result;
}

__s32 esTIME_ContiCntr(__hdle hCntr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_ContiCntr), (long)hCntr);
    return result;
}

__s32 esTIME_SetCntrValue(__hdle hCntr, __u32 value)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_TIME(esTIME_SetCntrValue), (long)hCntr, (long)value);
    return result;
}

__u32 esTIME_QuerryCntr(__hdle hCntr)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_TIME(esTIME_QuerryCntr), (long)hCntr);
    return result;
}

__s32 esTIME_SetCntrPrescale(__hdle hCntr, __s32 prescl)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_TIME(esTIME_SetCntrPrescale), (long)hCntr, (long)prescl);
    return result;
}

__s32 esTIME_QuerryCntrStat(__hdle hCntr)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_TIME(esTIME_QuerryCntrStat), (long)hCntr);
    return result;
}

// SVC
__s32 esSVC_RegCreatePath(const char *path)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegCreatePath), (long)path);
    return result;
}

__s32 esSVC_RegDeletePath(const char *path)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegDeletePath), (long)path);
    return result;
}

__hdle esSVC_RegOpenNode(const char *node, __s32 mode)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_SVC(esSVC_RegOpenNode), (long)node, (long)mode);
    return result;
}

__s32 esSVC_RegCloseNode(__hdle handle)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegCloseNode), (long)handle);
    return result;
}

__s32 esSVC_RegDeleteNode(const  char *node)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegDeleteNode), (long)node);
    return result;
}

__s32 esSVC_RegCreateSet(__hdle handle, const char *set_name)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_SVC(esSVC_RegCreateSet), (long)handle, (long)set_name);
    return result;
}

__s32 esSVC_RegDeleteSet(__hdle handle, const char *set_name)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_SVC(esSVC_RegDeleteSet), (long)handle, (long)set_name);
    return result;
}

__s32 esSVC_RegGetSetCount(__hdle handle, __u32 *count_p)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_SVC(esSVC_RegGetSetCount), (long)handle, (long)count_p);
    return result;
}

__s32 esSVC_RegGetFirstSet(__hdle handle)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegGetFirstSet), (long)handle);
    return result;
}

__s32 esSVC_RegGetNextSet(__hdle handle, char *set_name)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_SVC(esSVC_RegGetNextSet), (long)handle, (long)set_name);
    return result;
}

__s32 esSVC_RegCreateKey(__hdle hNode, const char *set_name, const char *key_name, const char *key_value)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_SVC(esSVC_RegCreateKey), (long)hNode, (long)set_name, (long)key_name, (long)key_value);
    return result;
}

__s32 esSVC_RegDeleteKey(__hdle hNode, const char *set_name, const char *key_name)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_SVC(esSVC_RegDeleteKey), (long)hNode, (long)set_name, (long)key_name);
    return result;
}

__s32 esSVC_RegGetKeyCount(__hdle handle, const char *set_name, __u32 *count_p)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_SVC(esSVC_RegGetKeyCount), (long)handle, (long)set_name, (long)count_p);
    return result;
}

__s32 esSVC_RegGetFirstKey(__hdle handle, const char *set_name)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_SVC(esSVC_RegGetFirstKey), (long)handle, (long)set_name);
    return result;
}

__s32 esSVC_RegGetNextKey(__hdle hNode, const char *set_name, char *key_name)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_SVC(esSVC_RegGetNextKey), (long)hNode, (long)set_name, (long)key_name);
    return result;
}

__s32 esSVC_RegSetKeyValue(__hdle hNode, const char *set_name, const char *key_name, const char *key_value)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_SVC(esSVC_RegSetKeyValue), (long)hNode, (long)set_name, (long)key_name, (long)key_value);
    return result;
}

__s32 esSVC_RegGetKeyValue(__hdle hNode, const char *set_name, const char *key_name, char *key_value)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_SVC(esSVC_RegGetKeyValue), (long)hNode, (long)set_name, (long)key_name, (long)key_value);
    return result;
}

__s32 esSVC_RegIni2Reg(const char *ini_file)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegIni2Reg), (long)ini_file);
    return result;
}

__s32 esSVC_RegReg2Ini(const char *ini_file)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegReg2Ini), (long)ini_file);
    return result;
}

void esSVC_RegSetRootPath(const char *path)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_SVC(esSVC_RegSetRootPath), (long)path);
    //return result;
}

void esSVC_RegGetRootPath(char *path)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegGetRootPath), (long)path);
    //return result;
}

__s32 esSVC_RegGetError(__hdle handle)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegGetError), (long)handle);
    return result;
}

__s32 esSVC_RegClrError(__hdle handle)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_RegClrError), (long)handle);
    return result;
}

__hdle esSVC_ResourceReq(__u32 res, __u32 mode, __u32 timeout)
{
    __hdle result ;
    result = (__hdle)__syscall3((long)SYSCALL_SVC(esSVC_ResourceReq), (long)res, (long)mode, (long)timeout);
    return result;
}

__s32 esSVC_ResourceRel(__hdle hRes)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_SVC(esSVC_ResourceRel), (long)hRes);
    return result;
}

// DEV
__s32 esDEV_Plugin(char *plgmsg, __u32 devno, void *p_arg, __u8 prio)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_DEV(esDEV_Plugin), (long)plgmsg, (long)devno, (long)p_arg, (long)prio);
    return result;
}

__s32 esDEV_Plugout(char *plgmsg, __u32 devno)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_DEV(esDEV_Plugout), (long)plgmsg, (long)devno);
    return result;
}

__hdle esDEV_DevReg(const char *classname, const char *name, const __dev_devop_t *pDevOp, void *pOpenArg)
{
    __hdle result ;
    result = (__hdle)__syscall4((long)SYSCALL_DEV(esDEV_DevReg), (long)classname, (long)name, (long)pDevOp, (long)pOpenArg);
    return result;
}

__s32 esDEV_DevUnreg(__hdle hNode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_DEV(esDEV_DevUnreg), (long)hNode);
    return result;
}
__hdle esDEV_Open(__hdle hNode, __u32 Mode)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_DEV(esDEV_Open), (long)hNode, (long)Mode);
    return result;
}

__s32 esDEV_Close(__hdle hDev)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_DEV(esDEV_Close), (long)hDev);
    return result;
}

__u32 esDEV_Read(void *pdata, __u32 size, __u32 n, __hdle hDev)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_DEV(esDEV_Read), (long)pdata, (long)size, (long)n, (long)hDev);
    return result;
}

__u32 esDEV_Write(const void *pdata, __u32 size, __u32 n, __hdle hDev)
{
    __u32 result ;
    result = (__u32)__syscall4((long)SYSCALL_DEV(esDEV_Write), (long)pdata, (long)size, (long)n, (long)hDev);
    return result;
}

__s32 esDEV_Ioctl(__hdle hDev, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_DEV(esDEV_Ioctl), (long)hDev, (long)cmd, (long)aux, (long)pbuffer);
    return result;
}

__s32 esDEV_Lock(__hdle hNode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_DEV(esDEV_Lock), (long)hNode);
    return result;
}

__s32 esDEV_Unlock(__hdle hNode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_DEV(esDEV_Unlock), (long)hNode);
    return result;
}

__s32 esDEV_Insmod(char *modfile, __u32 devno, void *p_arg)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_DEV(esDEV_Insmod), (long)modfile, (long)devno, (long)p_arg);
    return result;
}

__s32 esDEV_Unimod(char *modfile, __u32 devno)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_DEV(esDEV_Unimod), (long)modfile, (long)devno);
    return result;
}

// KRSV
__s32 esKSRV_SysInfo(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_KSRV(esKSRV_SysInfo));
    return result;
}

__s32 esKSRV_SendMsgEx(void *msg)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KSRV(esKSRV_SendMsgEx), (long)msg);
    return result;
}

__u32 esKSRV_GetMsg(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KSRV(esKSRV_GetMsg));
    return result;
}

__u32 esKSRV_GetVersion(__epdk_ver_type_t type)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_KSRV(esKSRV_GetVersion), (long)type);
    return result;
}

__u32 esKSRV_Random(__u32 max)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_KSRV(esKSRV_Random), (long)max);
    return result;
}

void esKSRV_Reset(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KSRV(esKSRV_Reset));
    return;
}

__u8 esKSRV_GetSocID(void)
{
    __u8 result ;
    result = (__u8)__syscall0((long)SYSCALL_KSRV(esKSRV_GetSocID));
    return result;
}

void esKSRV_PowerOff(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KSRV(esKSRV_PowerOff));
    return;
}

__s32 esKSRV_SendMsg(__u32 msgid, __u32 prio)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_KSRV(esKSRV_SendMsg), (long)msgid, (long)prio);
    return result;
}

__s32 esKSRV_GetTargetPara(__target_para_t *targetpara)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KSRV(esKSRV_GetTargetPara), (long)targetpara);
    return result;
}

__s32 esKSRV_EnableWatchDog(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_KSRV(esKSRV_EnableWatchDog));
    return result;
}

__s32 esKSRV_DisableWatchDog(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_KSRV(esKSRV_DisableWatchDog));
    return result;
}

__s32 esKSRV_ClearWatchDog(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_KSRV(esKSRV_ClearWatchDog));
    return result;
}

__s32 esKSRV_EnableWatchDogSrv(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_KSRV(esKSRV_EnableWatchDogSrv));
    return result;
}

__s32 esKSRV_DisableWatchDogSrv(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_KSRV(esKSRV_DisableWatchDogSrv));
    return result;
}

__s32 esKSRV_memcpy(void *pdest, const void *psrc, size_t size)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_KSRV(esKSRV_memcpy), (long)pdest, (long)psrc, (long)size);
    return result;
}

__u32 esKSRV_GetLowMsg(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KSRV(esKSRV_GetLowMsg));
    return result;
}

__u32 esKSRV_GetHighMsg(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KSRV(esKSRV_GetHighMsg));
    return result;
}

__s32 esKSRV_GetPara(__u32 type, void *format, void *para)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_KSRV(esKSRV_GetPara), (long)type, (long)format, (long)para);
    return result;
}

__s32 esKSRV_GetDramCfgPara(__dram_para_t *drampara)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KSRV(esKSRV_GetDramCfgPara), (long)drampara);
    return result;
}

__s32 esKSRV_memset(void *pmem, __u8 value, size_t size)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_KSRV(esKSRV_memset), (long)pmem, (long)value, (long)size);
    return result;
}

__s32 esKSRV_GetAddPara(__ksrv_add_para *ksrv_add_para)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KSRV(esKSRV_GetAddPara), (long)ksrv_add_para);
    return result;
}

__u32 esKSRV_close_logo(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KSRV(esKSRV_close_logo));
    return result;
}

__u32 esKSRV_release_logo_buf(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_KSRV(esKSRV_release_logo_buf));
    return result;
}

// PINS
__hdle esPINS_PinGrpReq(user_gpio_set_t *pGrpStat, __u32 GrpSize)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_PINS(esPINS_PinGrpReq), (long)pGrpStat, (long)GrpSize);
    return result;
}

__s32 esPINS_PinGrpRel(__hdle hPin, __s32 bRestore)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_PINS(esPINS_PinGrpRel), (long)hPin, (long)bRestore);
    return result;
}

__s32 esPINS_GetPinGrpStat(__hdle hPin, user_gpio_set_t *pGrpStat, __u32 GrpSize, __bool bFromHW)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_PINS(esPINS_GetPinGrpStat), (long)hPin, (long)pGrpStat, (long)GrpSize, (long)bFromHW);
    return result;
}

__s32 esPINS_GetPinStat(__hdle hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bFromHW)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_PINS(esPINS_GetPinStat), (long)hPin, (long)pPinStat, (long)pPinName, (long)bFromHW);
    return result;
}

__s32 esPINS_SetPinStat(__hdle hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bSetUserStat)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_PINS(esPINS_SetPinStat), (long)hPin, (long)pPinStat, (long)pPinName, (long)bSetUserStat);
    return result;
}

__s32 esPINS_SetPinIO(__hdle hPin, __bool bOut, const char *pPinName)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_PINS(esPINS_SetPinIO), (long)hPin, (long)bOut, (long)pPinName);
    return result;
}

__s32 esPINS_SetPinPull(__hdle hPin, __u32 PullStat, const char *pPinName)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_PINS(esPINS_SetPinPull), (long)hPin, (long)PullStat, (long)pPinName);
    return result;
}

__s32 esPINS_SetPinDrive(__hdle hPin, __u32 DriveLevel, const char *pPinName)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_PINS(esPINS_SetPinDrive), (long)hPin, (long)DriveLevel, (long)pPinName);
    return result;
}

__s32 esPINS_ReadPinData(__hdle hPin, const char *pPinName)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_PINS(esPINS_ReadPinData), (long)hPin, (long)pPinName);
    return result;
}

__s32 esPINS_WritePinData(__hdle hPin, __u32 Value, const char *pPinName)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_PINS(esPINS_WritePinData), (long)hPin, (long)Value, (long)pPinName);
    return result;
}

__s32 esPINS_EnbaleInt(__hdle hPin)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_PINS(esPINS_EnbaleInt), (long)hPin);
    return result;
}

__s32 esPINS_DisbaleInt(__hdle hPin)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_PINS(esPINS_DisbaleInt), (long)hPin);
    return result;
}

__s32 esPINS_QueryInt(__hdle hPin, __bool *pIntStatus)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_PINS(esPINS_QueryInt), (long)hPin, (long)pIntStatus);
    return result;
}

__s32 esPINS_SetIntMode(__hdle hPin, __u32 IntMode)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_PINS(esPINS_SetIntMode), (long)hPin, (long)IntMode);
    return result;
}

__s32 esPINS_RegIntHdler(__hdle hPin, __pCBK_t hdler, void *arg)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_PINS(esPINS_RegIntHdler), (long)hPin, (long)hdler, (long)arg);
    return result;
}

__s32 esPINS_UnregIntHdler(__hdle hPin, __pCBK_t hdler)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_PINS(esPINS_UnregIntHdler), (long)hPin, (long)hdler);
    return result;
}

__s32 esPINS_ClearPending(__hdle hPin)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_PINS(esPINS_ClearPending), (long)hPin);
    return result;
}

// CLK
__s32 esCLK_SetSrcFreq(__u32 nSclkNo, __u32 nFreq)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CLK(esCLK_SetSrcFreq), (long)nSclkNo, (long)nFreq);
    return result;
}

__u32 esCLK_GetSrcFreq(__u32 nSclkNo)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_CLK(esCLK_GetSrcFreq), (long)nSclkNo);
    return result;
}

__hdle esCLK_OpenMclk(__u32 nMclkNo)
{
    __hdle result ;
    result = (__hdle)__syscall1((long)SYSCALL_CLK(esCLK_OpenMclk), (long)nMclkNo);
    return result;
}

__s32 esCLK_CloseMclk(__hdle hMclk)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_CLK(esCLK_CloseMclk), (long)hMclk);
    return result;
}

__s32 esCLK_MclkRegCb(__u32 nMclkNo, __pCB_ClkCtl_t pCb)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CLK(esCLK_MclkRegCb), (long)nMclkNo, (long)pCb);
    return result;
}

__s32 esCLK_MclkUnregCb(__u32 nMclkNo, __pCB_ClkCtl_t pCb)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CLK(esCLK_MclkUnregCb), (long)nMclkNo, (long)pCb);
    return result;
}

__s32 esCLK_SetMclkSrc(__hdle hMclk, __s32 nSclkNo)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CLK(esCLK_SetMclkSrc), (long)hMclk, (long)nSclkNo);
    return result;
}

__s32 esCLK_GetMclkSrc(__hdle hMclk)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_CLK(esCLK_GetMclkSrc), (long)hMclk);
    return result;
}

__s32 esCLK_SetMclkDiv(__hdle hMclk, __s32 nDiv)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CLK(esCLK_SetMclkDiv), (long)hMclk, (long)nDiv);
    return result;
}

__s32 esCLK_GetMclkDiv(__hdle hMclk)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_CLK(esCLK_GetMclkDiv), (long)hMclk);
    return result;
}

__s32 esCLK_MclkOnOff(__hdle hMclk, __s32 bOnOff)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CLK(esCLK_MclkOnOff), (long)hMclk, (long)bOnOff);
    return result;
}

__s32 esCLK_MclkReset(__hdle hMclk, __s32 bReset)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CLK(esCLK_MclkReset), (long)hMclk, (long)bReset);
    return result;
}

void esCLK_SysInfo(const char *name)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_CLK(esCLK_SysInfo), (long)name);
    // return result;
}

void esCLK_ModInfo(const char *name)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_CLK(esCLK_ModInfo), (long)name);
    // return result;
}

// MEM
__s32 esMEM_DramWakeup(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_MEM(esMEM_DramWakeup));
    return result;
}

__s32 esMEM_DramSuspend(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_MEM(esMEM_DramSuspend));
    return result;
}

__hdle esMEM_RegDramAccess(__u8 dev_type, __pCB_ClkCtl_t dram_access)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_MEM(esMEM_RegDramAccess), (long)dev_type, (long)dram_access);
    return result;
}

__s32 esMEM_UnRegDramAccess(__hdle user)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_MEM(esMEM_UnRegDramAccess), (long)user);
    return result;
}

__s32 esMEM_MasterSet(__dram_dev_e mod, __dram_master_t *master)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_MEM(esMEM_MasterSet), (long)mod, (long)master);
    return result;
}

__s32 esMEM_MasterGet(__dram_dev_e mod, __dram_master_t *master)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_MEM(esMEM_MasterGet), (long)mod, (long)master);
    return result;
}

__s32 esMEM_RequestDramUsrMode(__dram_user_mode_t mode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_MEM(esMEM_RequestDramUsrMode), (long)mode);
    return result;
}

__s32 esMEM_ReleaseDramUsrMode(__dram_user_mode_t mode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_MEM(esMEM_ReleaseDramUsrMode), (long)mode);
    return result;
}

__s32 esMEM_BWEnable(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_MEM(esMEM_BWEnable));
    return result;
}

__s32 esMEM_BWDisable(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_MEM(esMEM_BWDisable));
    return result;
}

__s32 esMEM_BWGet(__bw_dev_e mod)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_MEM(esMEM_BWGet), mod);
    return result;
}

__hdle esMEM_SramReqBlk(__sram_block_e uBlock, __sram_req_mode_e uMode)
{
    __hdle result ;
    result = (__hdle)__syscall2((long)SYSCALL_MEM(esMEM_SramReqBlk), (long)uBlock, (long)uMode);
    return result;
}

__s32 esMEM_SramRelBlk(__hdle hSram)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_MEM(esMEM_SramRelBlk), (long)hSram);
    return result;
}

__s32 esMEM_SramSwitchBlk(__hdle hSram, __sram_block_map_e uMap)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_MEM(esMEM_SramSwitchBlk), (long)hSram, (long)uMap);
    return result;
}

// HID
__s32 esHID_SendMsg(__u32 msgid)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_HID(esHID_SendMsg), (long)msgid);
    return result;
}

__s32 esHID_hiddevreg(__hdle hNode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_HID(esHID_hiddevreg), (long)hNode);
    return result;
}

__s32 esHID_hiddevunreg(__hdle hNode, __u32 mode)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_HID(esHID_hiddevunreg), (long)hNode, (long)mode);
    return result;
}

// PWM
__s32 esPWM_RequestPwmMode(__s32 mode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_PWM(esPWM_RequestPwmMode), (long)mode);
    return result;
}

__s32 esPWM_ReleasePwmMode(__s32 mode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_PWM(esPWM_ReleasePwmMode), (long)mode);
    return result;
}

void esPWM_EnterStandby(__u32 power_off_flag)
{
    __u32 result ;
    result = (__u32)__syscall1((long)SYSCALL_PWM(esPWM_EnterStandby), (long)power_off_flag);
    return;
}

void esPWM_UsrEventNotify(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_PWM(esPWM_UsrEventNotify));
    return;
}

__s32 esPWM_LockCpuFreq(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_PWM(esPWM_LockCpuFreq));
    return result;
}

__s32 esPWM_UnlockCpuFreq(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_PWM(esPWM_UnlockCpuFreq));
    return result;
}

__s32 esPWM_RegDevice(__sys_pwm_dev_e device, __pCB_DPMCtl_t cb, void *parg)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_PWM(esPWM_RegDevice), (long)device, (long)cb, (long)parg);
    return result;
}

__s32 esPWM_UnregDevice(__sys_pwm_dev_e device, __pCB_DPMCtl_t cb)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_PWM(esPWM_UnregDevice), (long)device, (long)cb);
    return result;
}

__s32 esPWM_GetStandbyPara(__sys_pwm_para_t  *pStandbyPara)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_PWM(esPWM_GetStandbyPara), (long)pStandbyPara);
    return result;
}

__s32 esPWM_SetStandbyMode(__u32 standby_mode)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_PWM(esPWM_SetStandbyMode), (long)standby_mode);
    return result;
}

// CHS
__s32 esCHS_Uni2Char(__s32 type, __u16 uni, __u8 *str, __u32 len)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_CHS(esCHS_Uni2Char), (long)type, (long)uni, (long)str, (long)len);
    return result;
}

__s32 esCHS_Char2Uni(__s32 type, const __u8 *str, __u32 len, __u16 *uni)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_CHS(esCHS_Char2Uni), (long)type, (long)str, (long)len, (long)uni);
    return result;
}

__u32 esCHS_GetChUpperTbl(__s32 type, void *buf, __u32 size)
{
    __u32 result ;
    result = (__u32)__syscall3((long)SYSCALL_CHS(esCHS_GetChUpperTbl), (long)type, (long)buf, (long)size);
    return result;
}

__u32 esCHS_GetChLowerTbl(__s32 charset_type, void *buf, __u32 size)
{
    __u32 result ;
    result = (__u32)__syscall3((long)SYSCALL_CHS(esCHS_GetChLowerTbl), (long)charset_type, (long)buf, (long)size);
    return result;
}

// MSTUB
__s32 esMSTUB_RegFuncTbl(__s32 id, void *tbl)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_MSTUB(esMSTUB_RegFuncTbl), (long)id, (long)tbl);
    return result;
}

__s32 esMSTUB_UnRegFuncTbl(__s32 id)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_MSTUB(esMSTUB_UnRegFuncTbl), (long)id);
    return result;
}

__u32 esMSTUB_GetFuncEntry(__s32 id, __u32 funcnum)
{
    __u32 result ;
    result = (__u32)__syscall2((long)SYSCALL_MSTUB(esMSTUB_GetFuncEntry), (long)id, (long)funcnum);
    return result;
}

// INPUT
__s32 esINPUT_RegDev(__input_dev_t *dev)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_INPUT(esINPUT_RegDev), (long)dev);
    return result;
}

__s32 esINPUT_UnregDev(__input_dev_t *dev)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_INPUT(esINPUT_UnregDev), (long)dev);
    return result;
}

__hdle esINPUT_LdevGrab(char *ldev, __pCBK_t callback, void *pArg, __s32 aux)
{
    __hdle result ;
    result = (__hdle)__syscall4((long)SYSCALL_INPUT(esINPUT_LdevGrab), (long)ldev, (long)callback, (long)pArg, (long)aux);
    return result;
}

__s32 esINPUT_LdevRelease(__hdle graber)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_INPUT(esINPUT_LdevRelease), (long)graber);
    return result;
}

__s32 esINPUT_SendEvent(__input_dev_t *dev, __u32 type, __u32 code, __s32 value)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_INPUT(esINPUT_SendEvent), (long)dev, (long)type, (long)code, (long)value);
    return result;
}

__s32 esINPUT_LdevFeedback(__hdle graber, __input_event_packet_t *packet)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_INPUT(esINPUT_LdevFeedback), (long)graber, (long)packet);
    return result;
}

__s32 esINPUT_GetLdevID(__hdle graber)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_INPUT(esINPUT_GetLdevID), (long)graber);
    return result;
}

__s32 esINPUT_LdevCtl(__s32 LdevId, __s32 cmd, __s32 aux, void *pBuffer)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_INPUT(esINPUT_LdevCtl), (long)LdevId, (long)cmd, (long)aux, (long)pBuffer);
    return result;
}

// CONFIG
__s32 esCFG_GetKeyValue(char *SecName, char *KeyName, __s32 Value[], __s32 Count)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_CFG(esCFG_GetKeyValue), (long)SecName, (long)KeyName, (long)Value, (long)Count);
    return result;
}

__s32 esCFG_GetSecKeyCount(char *SecName)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_CFG(esCFG_GetSecKeyCount), (long)SecName);
    return result;
}

__s32 esCFG_GetSecCount(void)
{
    __s32 result ;
    result = (__s32)__syscall0((long)SYSCALL_CFG(esCFG_GetSecCount));
    return result;
}

__s32 esCFG_GetGPIOSecKeyCount(char *GPIOSecName)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_CFG(esCFG_GetGPIOSecKeyCount), (long)GPIOSecName);
    return result;
}

__s32 esCFG_GetGPIOSecData(char *GPIOSecName, void *pGPIOCfg, __s32 GPIONum)
{
    __s32 result ;
    result = (__s32)__syscall3((long)SYSCALL_CFG(esCFG_GetGPIOSecData), (long)GPIOSecName, (long)pGPIOCfg, (long)GPIONum);
    return result;
}

H_PARSER esCFG_Init_Ex(char *path)
{
    H_PARSER result ;
    result = (H_PARSER)__syscall1((long)SYSCALL_CFG(esCFG_Init_Ex), (long)path);
    return result;
}

__s32 esCFG_Exit_Ex(H_PARSER parser)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_CFG(esCFG_Exit_Ex), (long)parser);
    return result;
}

__s32 esCFG_GetKeyValue_Ex(H_PARSER parser, char *KeyName, __s32 Value[], __s32 Count)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_CFG(esCFG_GetKeyValue_Ex), (long)parser, (long)KeyName, (long)Value, (long)Count);
    return result;
}

__s32 esCFG_GetSecKeyCount_Ex(H_PARSER parser, char *SecName)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CFG(esCFG_GetSecKeyCount_Ex), (long)parser, (long)SecName);
    return result;
}

__s32 esCFG_GetSecCount_Ex(H_PARSER parser)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_CFG(esCFG_GetSecCount_Ex), (long)parser);
    return result;
}

__s32 esCFG_GetGPIOSecKeyCount_Ex(H_PARSER parser, char *GPIOSecName)
{
    __s32 result ;
    result = (__s32)__syscall2((long)SYSCALL_CFG(esCFG_GetGPIOSecKeyCount_Ex), (long)parser, (long)GPIOSecName);
    return result;
}

__s32 esCFG_GetGPIOSecData_Ex(H_PARSER parser, char *GPIOSecName, void *pGPIOCfg, __s32 GPIONum)
{
    __s32 result ;
    result = (__s32)__syscall4((long)SYSCALL_CFG(esCFG_GetGPIOSecData_Ex), (long)parser, (long)GPIOSecName, (long)pGPIOCfg, (long)GPIONum);
    return result;
}
