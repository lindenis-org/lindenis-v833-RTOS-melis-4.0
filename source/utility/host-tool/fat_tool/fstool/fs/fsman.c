/*
**********************************************************************************************************************
*                                                   ePOS
*                                  the Easy Portable/Player Operation System
*                                              fsys sub-system
*                                           vfs layer(vfs + vpart)
*
*                                (c) Copyright 2007-2008, Steven.ZGJ.China
*                                           All Rights Reserved
*
* File    : fsys.c
* By      : steven.ZGJ
* Version : V1.00
**********************************************************************************************************************
*/


#include "fsys_debug.h"
#include "err.h"
#include "nls.h"


/* Charset used for input and display */
struct nls_table            *nls = NULL;

__krnl_event_t              *pFSSem;
struct file_system_type     *pFSRoot = NULL;
__s32                        fs_err = 0;

extern int init_nls_cp936(void);
extern void exit_nls_cp936(void);
extern int buffer_init(void);
extern int bdflush_init(void);
extern int kswap_init(void);
extern int kmem_cache_init(void);
extern int kmem_cache_sizes_init(void);

/*
**********************************************************************************************************************
*
*             FS_Init
*
*  Description:
*  API function. Start the file system.
*
*  Parameters:
*  None.
*
*  Return value:
*  ==0         - File system has been started.
*  !=0         - An error has occured.
**********************************************************************************************************************
*/

__s32 fsys_vfs_init(void)
{
    int   err = 0;

    pFSSem = esKRNL_SemCreate(1);
    err |= !pFSSem;

    err |= init_nls_cp936();
    err |= page_pool_ini();
    err |= kmem_cache_init();
    err |= kmem_cache_sizes_init();
    err |= vfs_caches_init(0);
    err |= buffer_init();

    nls = load_nls("cp936");

#if 0
    err |= bdflush_init();
    err |= kswap_init();
#endif

    if (err) return EPDK_FAIL;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*             FS_Exit
*
*  Description:
*  API function. Stop the file system.
*
*  Parameters:
*  None.
*
*  Return value:
*  ==0         - File system has been stopped.
*  !=0         - An error has occured.
**********************************************************************************************************************
*/

__s32 fsys_vfs_exit(void)
{
    exit_nls_cp936();
    page_pool_exit();
    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*
*             fsys_regist_part
*
*  Description:
*   注册块设备到文件系统中
*
*  Parameters:
*   pFullName   - Fully qualified name.
*   pFilename   - Address of a pointer, which is modified to point to
*                the file name part of pFullName.
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information table.
**********************************************************************************************************************
*/
__s32 esFSYS_fsreg(__hdle hFS)
{
    __u8   err;
    __s32  res;
    __s32  cpu_sr;
    struct file_system_type *p, *fs = (struct file_system_type *)hFS;

    fs_log_trace0("fsreg:");

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_error("fsreg err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    ENTER_CRITICAL(cpu_sr);
    /* 判断文件系统是否已经被注册 */
    for (p = pFSRoot; p; p = p->next)
    {
        if (eLIBs_strcmp(p->name, fs->name) == 0)
        {
            fs_log_info("fs already registered!\n");
            ENTER_CRITICAL(cpu_sr);
            fs_err = -EEXIST;
            res = EPDK_FAIL;
            goto out;
        }
    }

    /* 初始化超级块链 */
    INIT_LIST_HEAD(&fs->fs_supers);
    /* 文件系统用户为0                          */
    fs->use_cnt = 0;
    /* 将驱动置为有效驱动                       */
    fs->status = FS_STAT_ACTIVE;
    /* 将驱动挂接到pFSRoot上                    */
    fs->next   = pFSRoot;
    pFSRoot    = fs;

    EXIT_CRITICAL(cpu_sr);
    res        = EPDK_OK;

out:
    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", res);
    return res;
}
/*
**********************************************************************************************************************
*
*             fsys_regist_part
*
*  Description:
*   拔除块设备
*
*  Parameters:
*   pFullName   - Fully qualified name.
*   pFilename   - Address of a pointer, which is modified to point to
*                the file name part of pFullName.
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information table.
**********************************************************************************************************************
*/
__s32 esFSYS_fsunreg(__hdle hFS)
{
    __u8   err;
    __s32   cpu_sr, res;
    struct file_system_type *fs = (struct file_system_type *)hFS;
    struct file_system_type *p, **pp = &pFSRoot;

    fs_log_trace0("fsureg:");

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("fsunreg err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    ENTER_CRITICAL(cpu_sr);
    if (fs->use_cnt)
    {
        fs_log_info("file system busy!\n");
        EXIT_CRITICAL(cpu_sr);
        fs_err = -EBUSY;
        res = EPDK_FAIL;
        goto out;
    }

    for (p = *pp; p && (fs != p); pp = &(p->next), p = p->next);
    if (!p)
    {
        EXIT_CRITICAL(cpu_sr);
        fs_log_info("try to destroy a fs not exsit in fs list!\n");
        fs_err = -ENXIO;
        res = EPDK_FAIL;
        goto out;
    }

    fs->status = FS_STAT_INACTIVE;
    *pp = fs->next;

    EXIT_CRITICAL(cpu_sr);
    res = EPDK_OK;

out:
    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", res);
    return res;
}

__s32 esFSYS_mntfs(__hdle part)
{
    __u8  err;
    __s32 cpu_sr, res;
    char *partName;
    struct file_system_type *pFS;

    fs_log_trace0("mntfs:");

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("mntfs err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    esFSYS_pioctrl(part, PART_IOC_SYS_GETNAME, 0, &partName);

    pFS = pFSRoot;
    while (pFS)
    {
        if (pFS->identify && pFS->status == FS_STAT_ACTIVE)
        {

            ENTER_CRITICAL(cpu_sr);
            pFS->use_cnt++;
            EXIT_CRITICAL(cpu_sr);

            if (pFS->identify(part) == EPDK_TRUE)
            {
                /* 分区可以被识别   */
                if (pFS->mount(part) == EPDK_OK)
                {
                    /* 分区可以被挂接   */
                    ENTER_CRITICAL(cpu_sr);
                    pFS->use_cnt--;
                    EXIT_CRITICAL(cpu_sr);
                    break;
                }
            }
            ENTER_CRITICAL(cpu_sr);
            pFS->use_cnt--;
            EXIT_CRITICAL(cpu_sr);
        }
        pFS = pFS->next;
    }
    if (!pFS)
    {
        fs_log_info("file system: null.\n");
        fs_err = -ENOEXEC;
        res = EPDK_FAIL;
    }
    else
    {
        fs_log_info("file system: \"%s\".\n", pFS->name);
        res =   EPDK_OK;
    }
out:
    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", res);
    return res;
}

__s32 esFSYS_umntfs(__hdle part, __s32 force)
{
    __u8   err;
    __s32  cpu_sr, res;
    struct file_system_type *pFS;
    struct super_block *sb;

    fs_log_trace0("umntfs:");

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_error("umntfs err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    /* 从文件系统上unmount分区  */
    esFSYS_pioctrl(part, PART_IOC_SYS_GETFSPRIV, 0, &sb);
    if (sb)
    {
        pFS = sb->s_type;
        if (pFS && pFS->unmount)
        {
            if (pFS->unmount(sb, force) == EPDK_FAIL)
            {
                if (force)
                    fs_log_error("force umount: unexpect error meet.\n");
                else
                    fs_log_info("non-force umount: %c is busy.\n", sb->s_letter);
                fs_err = -EBUSY;
                res = EPDK_FAIL;
                goto out;
            }
            else
            {
                char  letter;

                /* suppber block可能已经被当场销毁，需要从分区数
                 * 据结构中获取分区名
                 */
                esFSYS_pioctrl(part, PART_IOC_SYS_GETLETTER, 0, &letter);
                fs_log_info("part \"%c\" is unmounted from"
                            "\"%s\" fs.\n", letter, pFS->name);
            }
        }
    }
    esFSYS_pioctrl(part, PART_IOC_SYS_SETFSPRIV, 0, NULL);
    res = EPDK_OK;

out:
    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d", res);
    return res;
}

__s32 test_and_freeze_partfs(__hdle h_sb)
{
    __s32           cpu_sr;
    struct super_block *sb = (struct super_block *)h_sb;

    if (!sb)
        return 1;

    shrink_dcache_for_umount(sb, 0);
    if (!invalidate_inodes(sb))
    {
        ENTER_CRITICAL(cpu_sr);
        sb->s_frozen++;
        EXIT_CRITICAL(cpu_sr);
        sb->s_flags &= ~MS_ACTIVE;
        return 1;
    }
    return 0;
}

void unfreezepart(__hdle h_sb)
{
    __s32           cpu_sr;
    struct super_block *sb = (struct super_block *)h_sb;

    if (!sb)
        return;

    ENTER_CRITICAL(cpu_sr);
    if (sb->s_frozen)
    {
        sb->s_frozen--;
        if (!sb->s_frozen)
            sb->s_flags |= MS_ACTIVE;
    }
    EXIT_CRITICAL(cpu_sr);
}

