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


#include "sys_fsys_i.h"
#include "err.h"
#include "fsys_debug.h"
#include "nls.h"
#include "part.h"

extern struct file_system_type *pFSRoot;

/*
**********************************************************************************************************************
*
*             esFSYS_format
*
*  Description:
*  format one partiton
*
*  Parameters:
*   ppartName   - like 'c' or 'd' etc.
*   fstype      - wanna filesystem type
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information table.
**********************************************************************************************************************
*/
__s32 esFSYS_format(const char *ppartName, const char *fstype, __hdle fmtpara)
{
    __u8   err;
    int    res;
    struct file_system_type *pFS;
    __fsys_part_t   *pPart;

    fs_log_trace0("fmt:%s", fstype);
    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("format err1 when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }
    /* 识别文件系统                 */
    for (pFS = pFSRoot; pFS; pFS = pFS->next)
    {
        if (!eLIBs_strcmp(pFS->name, fstype))
        {
            if (pFS->format)
            {
                break;
            }
            else
            {
                fs_log_info("format failed!\n%s filesystem has no format function!\n", fstype);
                res = EPDK_FAIL;
                goto out;
            }
        }
    }
    if (!pFS)
    {
        fs_log_info("format failed!\n%s filesystem not found!maybe you need to install the filesystem first.\n", fstype);
        res = EPDK_FAIL;
        goto out;
    }

    /*  识别对应的分区 */
    pPart = (__fsys_part_t *)esFSYS_findpart(ppartName);
    if (!pPart)
    {
        fs_log_warning("partition not installed!\n");
        pPart = NULL;
        goto out;
    }
    res = pFS->format((__hdle)pPart, fmtpara);
    if (res)
    {
        fs_log_info("format failed!\n");
        res = EPDK_FAIL;
        goto out;
    }
    fs_log_info("complete.\n\n");
    res = EPDK_OK;

out:
    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O\n");
    return res;
}

int esFSYS_statfs(const char *path, __hdle buf)
{
    char               *s;
    int                 res;
    struct super_block *sb;
    __u8                err;
    struct kstatfs     *stat = (struct kstatfs *)buf;

    fs_log_trace0("stafs:%s,", path);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("statfs err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    /* 获取由pFileName路径名中的盘符指定的分区指针，并析取盘符以外的路径名 */
    sb = path_to_sb(path, NULL);
    if (!sb)
    {
        fs_log_warning("part \"%c\" not accessable!\n", *path);
        res = EPDK_FAIL;
        fs_err = -ENOENT;
        goto out;
    }

    if (sb->s_op->statfs)
    {
        res = sb->s_op->statfs(sb, stat);
    }
    else
    {
        res = EPDK_FAIL;
        fs_err = -EPERM;
        fs_log_warning("no statfs operation!\n");
    }

out:
    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O\n");
    return res;
}

/*
**********************************************************************************************************************
*
*             esFSYS_partfslock
*
*  Description:
*               - lock one disk from been fs-unmounted.
*
*  Parameters:
*   diskname    - disk letter follow up with ":\", such as "c:\" , 'd:\' etc.
*
*  Return value:
*   EPDK_OK     - ok
*   EPDK_FAIL   - some thing error
**********************************************************************************************************************
*/
__s32 esFSYS_partfslck(char *partname)
{
    __u8                err;
    __s32               res;
    struct super_block *sb;

    fs_log_trace0("pfslk:%s", partname);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_error("partfslck err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    sb = path_to_sb(partname, NULL);
    if (!sb)
    {
        fs_log_warning("part \"%c\" can not be accessed!\n", *partname);
        res = EPDK_FAIL;
        fs_err = -ENOENT;
        goto out;
    }
    if (NULL != igrab(sb->s_root->d_inode))
        res = EPDK_OK;
    else
        res = EPDK_FAIL;

out:
    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", res);
    return res;
}

/*
**********************************************************************************************************************
*
*             esFSYS_partfsunlock
*
*  Description:
*               - give up hold the fs of one disk. the disk then can be unmount from fs
*
*  Parameters:
*   diskname    - disk letter follow up with ":\", such as "c:\" , 'd:\' etc.
*
*  Return value:
*   EPDK_OK     - ok
*   EPDK_FAIL   - some thing error.
**********************************************************************************************************************
*/
__s32 esFSYS_partfsunlck(char *partname)
{
    __u8                err;
    __s32               res;
    struct super_block *sb;

    fs_log_trace0("pfsulk:%s", partname);

    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_error("partfsunlck err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    sb = path_to_sb(partname, NULL);
    if (!sb)
    {
        fs_log_warning("part \"%c\" can not be accessed!\n", *partname);
        res = EPDK_FAIL;
        fs_err = -ENOENT;
        goto out;
    }

    if (atomic_read(&sb->s_root->d_inode->i_count) > 1)
        iput(sb->s_root->d_inode);

    res = EPDK_OK;

out:
    esKRNL_SemPost(pFSSem);

    fs_log_trace0("O%d\n", res);
    return res;
}

__s32 esFSYS_setfs(char *partname, __u32 cmd, __s32 aux, char *para)
{
    __u8                err;
    struct nls_table   *new_nls;

    fs_log_trace0("setfs: path(%s), cmd(%d), para(%s)", partname, cmd, para);
    esKRNL_SemPend(pFSSem, 0, &err);
    if (err)
    {
        fs_log_warning("setfs err when pend sem, err:%d\n", err);
        return EPDK_FAIL;
    }

    new_nls = load_nls(para);
    if (new_nls)
    {
        fs_log_info("Set nls to [%d]\n", aux);
        unload_nls(nls);
        nls = new_nls;
    }

    esKRNL_SemPost(pFSSem);
    fs_log_trace0("O\n");
    return EPDK_OK;
}
