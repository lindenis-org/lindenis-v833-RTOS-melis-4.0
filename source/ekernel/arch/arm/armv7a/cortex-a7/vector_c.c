/*
 * =====================================================================================
 *
 *       Filename:  vector_c.c
 *
 *    Description:  rotuine to dealt with exception. based on ARM926EJ-S
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 13:43:10
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-26 11:44:19
 *
 * =====================================================================================
 */

#include <typedef.h>
#include <sys_int.h>
#include <excep.h>
#include <kconfig.h>
#include <log.h>
#include <stdio.h>
#include <list.h>
#include <arch.h>
#include <debug.h>
#include <ktimer.h>

#ifdef CONFIG_RTTKERNEL
#include <rtthread.h>
#endif

#ifdef CONFIG_DEBUG_BACKTRACE
#include <backtrace.h>
#endif

#define DABT_TYPE               (0)
#define IABT_TYPE               (1)
#define UNDF_TYPE               (2)

#define FSR_LNX_PF              (1 << 31)
#define FSR_WRITE               (1 << 11)
#define FSR_FS4                 (1 << 10)
#define FSR_FS3_0               (15)
#define FSR_FS5_0               (0x3f)
#define FSR_FS_AEA              22

static void dump_system_information(void);

static inline int fsr_fs(unsigned int fsr)
{
    return (fsr & FSR_FS3_0) | (fsr & FSR_FS4) >> 6;
}
static char *fsr_info[] =
{
    /*
    ¦* The following are the standard ARMv3 and ARMv4 aborts.  ARMv5
    ¦* defines these to be "precise" aborts.
    ¦*/
    "vector exception",
    "alignment exception",
    "terminal exception",
    "alignment exception",
    "external abort on linefetch",
    "section translation fault",
    "external abort on linefetch",
    "page translation fault",
    "external abort on non-linefetch",
    "section domain fault",
    "external abort on non-linefetch",
    "page domain fault",
    "external abort on translation",
    "section permission fault",
    "external abort on translation",
    "page permission fault",
    "unknown 16",
    "unknown 17",
    "unknown 18",
    "unknown 19",
    "lock abort",                       /* xscale */
    "unknown 21",
    "imprecise external abort",         /* xscale */
    "unknown 23",
    "dcache parity error",              /* xscale */
    "unknown 25",
    "unknown 26",
    "unknown 27",
    "unknown 28",
    "unknown 29",
    "unknown 30",
    "unknown 31",
};

#if defined CONFIG_SUBSYS_KGDB

static LIST_HEAD(undef_hook);
static LIST_HEAD(data_abort_hook);
static LIST_HEAD(instr_prefetch_abort_hook);

void register_undef_hook(struct undef_hook *hook)
{
    unsigned long flags;

    list_add(&hook->node, &undef_hook);
}

void unregister_undef_hook(struct undef_hook *hook)
{
    unsigned long flags;

    list_del(&hook->node);
}

static int call_undef_hook(excep_regs_t *regs)
{
    struct undef_hook *hook;
    unsigned long flags;
    int (*fn)(excep_regs_t *regs) = NULL;
    unsigned long instr;

    if (regs->orig_cpsr & 0x20)
    {
        instr = (*(unsigned short *)regs->abt_lr) & 0xffff;
    }
    else
    {
        instr = *(unsigned long *)regs->abt_lr;
    }

    list_for_each_entry(hook, &undef_hook, node)
    {
        if ((instr & hook->instr_mask) == hook->instr_val)
        {
            fn = hook->fn;
            break;
        }
    }

    return fn ? fn(regs) : 1;
}

void register_data_abort_hook(struct data_abort_hook *hook)
{
    unsigned long flags;

    list_add(&hook->node, &data_abort_hook);
}

void unregister_data_abort_hook(struct data_abort_hook *hook)
{
    unsigned long flags;

    list_del(&hook->node);
}

static int call_data_abort_hook(excep_regs_t *regs)
{
    struct data_abort_hook *hook;
    unsigned long flags;
    int (*fn)(excep_regs_t *regs) = NULL;
    unsigned long abt_dfsr = (unsigned long)regs->abt_dFSR;

    list_for_each_entry(hook, &data_abort_hook, node)
    {
        if ((abt_dfsr & hook->dfsr_mask) == hook->dfsr_val)
        {
            fn = hook->fn;
            break;
        }
    }

    return fn ? fn(regs) : 1;
}

void register_instr_prefetch_abort_hook(struct instr_prefetch_abort_hook *hook)
{
    unsigned long flags;

    list_add(&hook->node, &instr_prefetch_abort_hook);
}

void unregister_instr_prefetch_abort_hook(struct instr_prefetch_abort_hook *hook)
{
    unsigned long flags;

    list_del(&hook->node);
}

static int call_instr_prefetch_abort_hook(excep_regs_t *regs)
{
    struct instr_prefetch_abort_hook *hook;
    unsigned long flags;
    int (*fn)(excep_regs_t *regs) = NULL;
    unsigned long abt_ifsr = (unsigned long)regs->abt_iFSR;

    list_for_each_entry(hook, &instr_prefetch_abort_hook, node)
    {
        if ((abt_ifsr & hook->ifsr_mask) == hook->ifsr_val)
        {
            fn = hook->fn;
            break;
        }
    }

    return fn ? fn(regs) : 1;
}

static int thumb_32bit_code(uint16_t ic)
{
    uint16_t op = (ic >> 11) & 0xFFFF;
    if (op == 0x1D || op == 0x1E || op == 0x1F)
    {
        return 1;
    }
    return 0;
}
#endif

static void dump_memory(uint32_t *buf, int32_t len)
{
    int i;

    rt_kprintf("\n\rdump stack memory:");
    for (i = 0; i < len; i ++)
    {
        if ((i % 4) == 0)
        {
            rt_kprintf("\n\r0x%p: ", buf + i);
        }
        rt_kprintf("0x%08lx ", buf[i]);
    }
    rt_kprintf("\n\r");

    return;
}

void memory_info(void)
{
#ifdef CONFIG_RTTKERNEL
    rt_uint32_t total, used, max_used;

    rt_memory_info(&total, &used, &max_used);

    rt_kprintf("\n\rmemory info:\n\r");
    rt_kprintf("    Total  0x%08x\n\r" \
               "    Used   0x%08x\r\n" \
               "    Max    0x%08x\r\n" \
               "    Margin 0x%08x\n\r", \
               total, used, max_used, total - max_used);
#endif
}

int g_cli_direct_read = 0;
static void panic_goto_cli(void)
{
    if (g_cli_direct_read > 0)
    {
        rt_kprintf("%s can not be reentrant!\n", __func__);
        return;
    }
    g_cli_direct_read = 1;
#ifdef CONFIG_RT_USING_FINSH
    void finsh_thread_entry(void *parameter);
    finsh_thread_entry(NULL);
#endif
}

/**
 * this function will show registers of CPU
 *
 * @param regs the registers point
 */

static void show_register(excep_regs_t *regs, uint32_t victim, int32_t type)
{
#ifdef CONFIG_RTTKERNEL
    rt_kprintf("thread: %s, entry: 0x%p, stack_base: 0x%p,stack_size: 0x%08x.\n", \
               rt_thread_self()->name, \
               rt_thread_self()->entry, \
               rt_thread_self()->stack_addr, \
               rt_thread_self()->stack_size);
#endif
    rt_kprintf("cpsr: 0x%08x.\n", regs->orig_cpsr);

    rt_kprintf("\ngprs:\n");
    rt_kprintf("r00:0x%08x r01:0x%08x r02:0x%08x r03:0x%08x\n",
               regs->r0, regs->r1, regs->r2, regs->r3);
    rt_kprintf("r04:0x%08x r05:0x%08x r06:0x%08x r07:0x%08x\n",
               regs->r4, regs->r5, regs->r6, regs->r7);
    rt_kprintf("r08:0x%08x r09:0x%08x r10:0x%08x r11:0x%08x\n",
               regs->r8, regs->r9, regs->r10, regs->r11);
    rt_kprintf("r12:0x%08x  sp:0x%08x  lr:0x%08x  pc:0x%08x\n",
               regs->r12, regs->sp, regs->lr, victim);

    rt_kprintf("\ncp15:\n");
    rt_kprintf("fst_fsid:0x%08x\n" \
               "abt_dfar:0x%08x\n" \
               "abt_ifsr:0x%08x\n" \
               "abt_dfsr:0x%08x\n" \
               "abt_lr  :0x%08x\n" \
               "abt_sp  :0x%p  \n", \
               regs->fcsid, regs->abt_dFAR, regs->abt_iFSR, regs->abt_dFSR, regs->abt_lr, regs);

    rt_kprintf("\ncause:\n");
    if (type == DABT_TYPE)
    {
        rt_kprintf("DFSR signs %s.\n",  fsr_info[fsr_fs(regs->abt_dFSR)]);
        void show_pte(unsigned long addr);
        show_pte(regs->abt_dFAR);
    }
    else if (type == IABT_TYPE)
    {
        rt_kprintf("IFSR signs %s.\n",  fsr_info[fsr_fs(regs->abt_iFSR)]);
    }
    else
    {
        rt_kprintf("undefind instructions.\n\r");
    }

    rt_kprintf("\r\n");

#ifdef CONFIG_DEBUG_BACKTRACE
    backtrace_exception((print_function)rt_kprintf, regs->orig_cpsr, regs->sp, regs->abt_lr, regs->lr);
#endif

    dump_system_information();

    dump_memory((uint32_t *)regs->sp, 128);

    rt_kprintf("\r\n");
    rt_kprintf("================================================================\n");

    panic_goto_cli();
}

/**
 * When ARM7TDMI comes across an instruction which it cannot handle,
 * it takes the undefined instruction trap.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void awos_arch_und_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr - 0;

#if defined CONFIG_SUBSYS_KGDB
    if (regs->orig_cpsr & 0x20)
    {
        uint16_t nextins;
        regs->abt_lr -= 2;
        nextins = (*(unsigned short *)(victim - 4)) & 0xffff;
        if (thumb_32bit_code(nextins) == 1)
        {
            regs->abt_lr -= 2;
        }
    }
    else
#endif
    {
        regs->abt_lr -= 4;
    }
#if defined CONFIG_SUBSYS_KGDB
    if (!call_undef_hook(regs))
    {
        return;
    }
#endif

    rt_kprintf("================================================================\n");
    rt_kprintf("                  undefined instructions trap                   \n");
    rt_kprintf("================================================================\n");

    show_register(regs, victim, UNDF_TYPE);
    regs->abt_lr = victim;

    while (i);
}

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during an instruction prefetch.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */

void awos_arch_iabt_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr;
#define readmem(addr)     (*(const volatile uint32_t *)addr)
#define readmem_thumb(addr)     (*(const volatile uint16_t *)addr)
#define accessable(addr)  ((((addr & 0xf0000000) >> 24) == 0xc0) || \
                           (((addr & 0xf0000000) >> 24) == 0xe0))

    regs->abt_lr -= 4;
#if defined CONFIG_SUBSYS_KGDB
    if (!call_instr_prefetch_abort_hook(regs))
    {
        return;
    }
#endif

    //arm mode.
    if (!(regs->orig_cpsr & ARMV7A_THB_MASK))
    {
        /*makesure in code range and check whether it was the 'bkpt' instructions.*/
        if (accessable(victim) && ((readmem(victim) & 0xfff00070) == 0xe1200070))
        {
            static uint32_t loop = 0;

            /* A proper intrval for print */
            if ((loop ++ & 0x007fffff) == 0)
            {
                rt_kprintf("arm:softbreak, connect the debugger.sp: 0x%p, lr: 0x%08x, abt_lr: 0x%08x...\n", \
                           regs, regs->lr, regs->abt_lr);
#ifdef CONFIG_DEBUG_BACKTRACE
                backtrace_exception((print_function)rt_kprintf, regs->orig_cpsr, regs->sp, regs->abt_lr, regs->lr);
#endif
            }

            /*resume to connect the DS5*/
            return;
        }
    }
    else  //thumb mode.
    {
        /*makesure in code range and check whether it was the 'bkpt' instructions.*/
        if (accessable(victim) && ((readmem_thumb(victim) & 0xff00) == 0xbe00))
        {
            static uint32_t loop = 0;

            /* A proper intrval for print */
            if ((loop ++ & 0x007fffff) == 0)
            {
                rt_kprintf("thumb:softbreak, connect the debugger.sp: 0x%p, lr: 0x%08x, abt_lr: 0x%08x...\n", \
                           regs, regs->lr, regs->abt_lr);
            }

            //force to thumb state, can be comment for cpsr already in the right state.
            regs->abt_lr |= 1;
            /*resume to connect the DS5*/
            return;
        }
    }
    rt_kprintf("================================================================\n");
    rt_kprintf("                      memory access abort(IF)                   \n");
    rt_kprintf("================================================================\n");

    show_register(regs, victim, IABT_TYPE);
    regs->abt_lr = victim;

    while (i);
}


/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during a data access.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void awos_arch_dabt_trap_entry(excep_regs_t *regs)
{
    volatile uint8_t  i = 1;
    uint32_t victim = regs->abt_lr - 8;

    regs->abt_lr -= 8;
#if defined CONFIG_SUBSYS_KGDB
    if (!call_data_abort_hook(regs))
    {
        return;
    }
#endif

    rt_kprintf("================================================================\n");
    rt_kprintf("                      memory access abort(MA)                   \n");
    rt_kprintf("================================================================\n");

    show_register(regs, victim, DABT_TYPE);
    regs->abt_lr = victim;

    while (i);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_entry, IRQ routine. */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_entry(irq_regs_t *regs)
{
    void gic_handle_irq(irq_regs_t *regs);
    if (!irqs_disabled())
    {
        __err("Do you wanna nest irq?");
        software_break();
    }

    gic_handle_irq(regs);

    if (!irqs_disabled())
    {
        __err("Irq opend by interrupt handler.");
        software_break();
    }
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_swc_func_error, wrong syscall report.
 *
 * @param[in] spsr
 * @param[in] cpsr
 * @param[in] swino
 * @param[in] swihander
 * @param[in] caller_lr
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_swc_func_error(uint32_t spsr, uint32_t cpsr, uint32_t swino, uint32_t swihander, uint32_t caller_lr)
{
    volatile uint8_t  i = 1;

    rt_kprintf("syscall error: invalid syscall handler.\n");
    rt_kprintf("================================================================\n");
    rt_kprintf("                      syscall error                             \n");
    rt_kprintf("================================================================\n");

    rt_kprintf("spsr      :0x%08x\n" \
               "cpsr      :0x%08x\n" \
               "swino     :0x%08x\n" \
               "swihandler:0x%08x\n" \
               "caller_lr :0x%08x\n", spsr, cpsr, swino, swihander, caller_lr);

    while (i);
}


/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_bad_stack_error <irq stack corrupted happened, stack balan-
 *         ce destroyed.>
 *
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_irqstack_balance_error(void)
{
    volatile uint8_t  i = 1;

    rt_kprintf("irq stack corrupted: invalid stack pointer.\n");
    rt_kprintf("================================================================\n");
    rt_kprintf("                      irq error                                 \n");
    rt_kprintf("================================================================\n");


    while (i);
}

void awos_arch_bad_switch_error(rt_thread_t old, rt_thread_t new)
{
    volatile uint8_t  i = 1;

    rt_kprintf("bad irq switch.\n");
    rt_kprintf("================================================================\n");
    rt_kprintf("                      bad irq switch                            \n");
    rt_kprintf("================================================================\n");

    rt_kprintf("old thread :%s\n" \
               "new thread :%s\n",  old->name, new->name);

    while (i);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_swc_mode_error <wrong cpu mode happed>
 *
 * @param spsr
 * @param cpsr
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_swc_mode_error(uint32_t spsr, uint32_t cpsr)
{
    volatile uint8_t  i = 1;

    rt_kprintf("syscall error: invalid caller cpu mode.\n");
    rt_kprintf("================================================================\n");
    rt_kprintf("                        syscall error                           \n");
    rt_kprintf("================================================================\n");

    rt_kprintf("spsr:0x%08x\ncpsr:0x%08x\n", spsr, cpsr);

    while (i);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_fiq_trap_entry <FIQ not support.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_fiq_trap_entry(void)
{
    volatile uint8_t  i = 1;

    rt_kprintf("fiq trap: fast interrupt did not support!\n");

    while (i);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_rst_trap_entry <reset mode> */
/* ----------------------------------------------------------------------------*/
void awos_arch_rst_trap_entry(void)
{
    volatile uint8_t  i = 1;

    rt_kprintf("reset trap: should never happen, brom stage.\n");

    while (i);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_rsv_trap_entry <reserved exception, should never happend> */
/* ----------------------------------------------------------------------------*/
void awos_arch_hyp_trap_entry(void)
{
    volatile uint8_t  i = 1;

    rt_kprintf("reserve trap: should never happen.\n");

    while (i);
}

static void dump_system_information(void)
{
    struct rt_object_information *information;
    struct rt_object *object;
    struct rt_list_node *node;
    rt_thread_t temp;
    rt_tick_t  duration;
    rt_uint32_t total, used, max_used;
    rt_uint8_t *ptr;
    char *stat;
    rt_ubase_t stk_free;

    rt_enter_critical();
    rt_kprintf("\r\n");
    rt_kprintf("    -----------------------------------------------TSK Usage Report----------------------------------------------\n");
    rt_kprintf("        name     errno    entry       stat   prio     tcb     slice stacksize      stkfree  lt     pc      si   so\n");

    information = rt_object_get_information(RT_Object_Class_Thread);
    RT_ASSERT(information != RT_NULL);
    for (node = information->object_list.next; node != &(information->object_list); node = node->next)
    {
        rt_uint8_t status;
        rt_uint32_t pc = 0xdeadbeef;
        switch_ctx_regs_t *regs_ctx;

        object = rt_list_entry(node, struct rt_object, list);
        temp = (rt_thread_t)object;

        if (temp != rt_thread_self())
        {
            regs_ctx = (switch_ctx_regs_t *)temp->sp;
        }
        else
        {
            regs_ctx = RT_NULL;
        }

        if (regs_ctx && (regs_ctx->use_fpu == 1))
        {
            pc = regs_ctx->context.have_neon.pc;
        }
        else if (regs_ctx && (regs_ctx->use_fpu == 0))
        {
            pc = regs_ctx->context.no_neon.pc;
        }
        else if (regs_ctx == RT_NULL)
        {
            pc = (rt_uint32_t)RT_NULL;
        }
        else
        {
            __err("fatal error, cant happend! thread: %s, regs_ctx: 0x%08x, fpu: %d.", temp->name, (rt_uint32_t)regs_ctx, regs_ctx->use_fpu);
            software_break();
        }

        status = (temp->stat & RT_THREAD_STAT_MASK);

        if (status == RT_THREAD_READY)
        {
            stat = "running";
        }
        else if (status == RT_THREAD_SUSPEND)
        {
            stat = "suspend";
        }
        else if (status == RT_THREAD_INIT)
        {
            stat = "initing";
        }
        else if (status == RT_THREAD_CLOSE)
        {
            stat = "closing";
        }
        else
        {
            stat = "unknown";
        }

        ptr = (rt_uint8_t *)temp->stack_addr;
        while (*ptr == '#')
        {
            ptr ++;
        }

        stk_free = (rt_uint32_t) ptr - (rt_uint32_t) temp->stack_addr;
        rt_kprintf("%15s%5ld   0x%08lx  %9s %4d   0x%08lx  %3ld %8d    %8ld    %02ld 0x%08x %04d %04d\n", \
                   temp->name,
                   temp->error,
                   (rt_ubase_t)temp->entry,
                   stat,
                   temp->current_priority,
                   (rt_ubase_t)temp,
                   temp->init_tick,
                   temp->stack_size,
                   stk_free,
                   temp->remaining_tick, pc, temp->sched_i, temp->sched_o);
    }

    rt_kprintf("    -------------------------------------------------------------------------------------------------------------------\n");
    rt_memory_info(&total, &used, &max_used);
    rt_kprintf("\n\r    memory info:\n\r");
    rt_kprintf("\tTotal  0x%08x\n\r" \
               "\tUsed   0x%08x\n\r" \
               "\tMax    0x%08x\n\r", \
               total, used, max_used);
    rt_kprintf("    ------------------------------------------------memory information-------------------------------------------------\n");

    rt_exit_critical();

    return;
}
