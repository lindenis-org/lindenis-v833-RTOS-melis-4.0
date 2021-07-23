/*
 * ===========================================================================================
 *
 *       Filename:  swi.c
 *
 *    Description:  syscall entry definition.
 *
 *        Version:  Melis3.0
 *         Create:  2018-04-24 11:11:07
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-04-27 17:24:50
 *
 * ===========================================================================================
 */

#include <typedef.h>
#include <rtthread.h>
#include <kconfig.h>
#include <mod_defs.h>
#include <stdio.h>
#include <const.h>
#include <rthw.h>
#include <kapi.h>
#include <debug.h>
#include <arch.h>
#include <log.h>
#include "epos.c"

unsigned int melis_syscall_table[256] = {0};
#ifdef CONFIG_PTHREAD
extern const uint32_t SWIHandler_PTHREAD[];
#endif

#if 0
#define  syscall_trap   (RT_NULL)
#else
static void syscall_trap(void)
{
    __err("syscall not support! from %p.", __builtin_return_address(0));
    software_break();
}
#endif

void esMEMS_CleanFlushDCacheRegion(void *adr, __u32 bytes)
{
    if (!adr || !bytes)
    {
        return;
    }

    if (((bytes % 32) != 0) || ((uint32_t)adr) & 0x1f)
    {
    }

#ifndef CONFIG_CPU_DCACHE_DISABLE
    awos_arch_mems_clean_flush_dcache_region((unsigned long)adr, bytes);
#endif
}

void esMEMS_CleanInvalidateCacheAll(void)
{
    awos_arch_flush_kern_cache_all();
}

void esMEMS_CleanDCache(void)
{
    return awos_arch_clean_dcache();
}

void esMEMS_CleanFlushDCache(void)
{
    return awos_arch_flush_dcache();
}

void esMEMS_CleanFlushCache(void)
{
    return awos_arch_clean_flush_cache();
}

void esMEMS_FlushDCache(void)
{
    return awos_arch_flush_dcache();
}

void esMEMS_FlushICache(void)
{
    return awos_arch_flush_icache();
}

void esMEMS_FlushCache(void)
{
    return awos_arch_flush_cache();
}

__u32 esMEMS_TotalMemSize(void)
{
    return DRAM_SIZE;
}

__u32 esMEMS_GetIoVAByPA(__u32 phyaddr, __u32 size)
{
    return awos_arch_bus_to_virt(phyaddr);
}

__u32 esMEMS_FreeMemSize(void)
{
    rt_uint32_t total, used, max_used;
    rt_uint32_t aval;

    rt_memory_info(&total, &used, &max_used);
    aval = total - used;

    __log("aval memory: 0x%08x.", aval);
    return aval;
}

void *esRESM_ROpen(const char *file, const char *mode)
{
    rt_uint8_t   res_head[16] = {0};
    __resm_rsb_t *hRes;

    if (file == RT_NULL)
    {
        __err("invalid file path for resthem open.");
        return RT_NULL;
    }

    //allocate resouce manager handler
    hRes = (__resm_rsb_t *)rt_malloc(sizeof(__resm_rsb_t));
    if (hRes == (__resm_rsb_t *)0)
    {
        __err("allocate buffer for res manager failed.");
        return RT_NULL;
    }

    // open res file
    hRes->hResfile = esFSYS_fopen(file, "r");
    if (hRes->hResfile == 0)
    {
        __err("cannot open file %s.", file);
        rt_free((void *)hRes);
        return RT_NULL;
    }
    //--------------------------------------------------------------------------
    //check if the file is a valid res file
    //--------------------------------------------------------------------------
    esFSYS_fread(res_head, 16, 1, hRes->hResfile);
    //check if the file contain valid res magic
    if (!rt_strncmp((char *)res_head, ".resthem", 8) ||
        !rt_strncmp((char *)res_head, ".langdat", 8))
    {
        //current file is a valid res file, set some prameter, and return
        esFSYS_fseek(hRes->hResfile, 0, FSYS_SEEK_END);
        hRes->size = esFSYS_ftell(hRes->hResfile);
        hRes->offset = 0;
        esFSYS_fseek(hRes->hResfile, 0, FSYS_SEEK_SET);

        if (hRes->size == 0)
        {
            __err("res file size is zero.");
        }
        return hRes;
    }

    __err("invalid resthem file [%s] access.", file);
    esFSYS_fclose(hRes->hResfile);
    rt_free((void *)hRes);

    return RT_NULL;
}

__s32 esRESM_RClose(__resm_rsb_t *res)
{
    if (!res || !res->hResfile)
    {
        __err("close res file failure.");
        return 0;
    }
    esFSYS_fclose(res->hResfile);
    rt_free((void *)res);
    return 0;
}

__u32 esRESM_RRead(void *pdata, __u32 size, __u32 n, __resm_rsb_t *res)
{
    if (!pdata || !res || !res->hResfile)
    {
        __err("read res file failure.");
        return 0;
    }
    return esFSYS_fread(pdata, size, n, res->hResfile);
}

__s32 esRESM_RSeek(__resm_rsb_t *res, __s32 Offset, __s32 Whence)
{
    if (!res || !res->hResfile)
    {
        __err("seek res file failure.");
        return 0;
    }
    return esFSYS_fseek(res->hResfile, Offset + res->offset, Whence);
}

void esKRNL_DumpStack(void)
{
    void dump_stack_nofp(unsigned int pc, unsigned int sp, int level, unsigned int *pointer, int flag);
    dump_stack_nofp(0, 0, 0, NULL, 0);
}

void esSIOS_foobar(int a, int b, int c, int d, int e, int f)
{
    __log("a = %d, b = %d, c = %d, d = %d, e = %d, f = %d.\n", a, b, c, d, e, f);
}
