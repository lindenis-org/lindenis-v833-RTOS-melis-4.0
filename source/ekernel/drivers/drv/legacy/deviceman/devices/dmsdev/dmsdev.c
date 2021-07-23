/*
**********************************************************************************************************************
*                                                   ePOS
*                                  the Easy Portable/Player Operation System
*
*                                (c) Copyright 2007-2008, Steven.ZGJ.China
*                                           All Rights Reserved
*
* File    : devman.c
* By      : steven.ZGJ
* Version : V1.00
**********************************************************************************************************************
*/

#include "dmsdev.h"
#include <sys_fsys.h>
#include <log.h>

static __dev_devop_t dmsdev_ops;
static __hdle dmshandle;
static struct dmsdev dms;
extern __dev_classnode_t devclass_nop;

__s32 dmsdev_init(void)
{
    dmshandle = esDEV_DevReg("DMS", "dms01", &dmsdev_ops, 0);
    if (dmshandle)
    {
        return EPDK_OK;
    }
    __err("dmsdev registered Error!");
    return EPDK_FAIL;
}

__s32 dmsdev_exit(void)
{
    return esDEV_DevUnreg(dmshandle);
}

static __hdle dmsdev_Open(void *open_arg, __u32 mode)
{
    dms.inuse = 1;
    return (__hdle)&dms;
}

static __s32 dmsdev_Close(__hdle hDev)
{
    if (dms.inuse)
    {
        dms.inuse = 0;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

static __u32 dmsdev_Read(void *pBuffer, __u32 size, __u32 n, __hdle hDev)
{
    return 0;
}

static __u32 dmsdev_Write(const void *pBuffer, __u32 size, __u32 n, __hdle hDev)
{
    return 0;
}

static __s32 dmsdev_Ioctrl(__hdle hDev, __u32 Cmd, __s32 Aux, void *pBuffer)
{
    switch (Cmd)
    {
        case DEV_IOC_USR_GET_DEVROOT:
        {
            *((int *)pBuffer) = (int)(&devclass_nop);
            return EPDK_OK;
        }

        case DEV_IOC_USR_GET_DEVITEM:
        {
            struct DmsDirInfo_dev *itemInfo = (struct DmsDirInfo_dev *)Aux;
            int i, res, idx = itemInfo->itemPos;
            __dev_classnode_t *pClassnode = (__dev_classnode_t *)itemInfo->dir;
            struct DmsNodeInfo_dev *dinfo = (struct DmsNodeInfo_dev *)pBuffer;

            /* 查找类顶点节点下的类节点，即一级目录 */
            if (itemInfo->dir == (__hdle)&devclass_nop)
            {
                for (i = 0, pClassnode = devclass_nop.next; pClassnode && i < idx; pClassnode = pClassnode->next, i ++)
                    ;
                if (!pClassnode)
                {
                    res = EPDK_FAIL;
                }
                else
                {
                    /* 返回指向的设备节点链 */
                    dinfo->name = pClassnode->name;
                    dinfo->hnode = (__hdle)(pClassnode);
                    dinfo->type = DEVFS_CLASS_TYPE;
                    res = EPDK_OK;
                }
            }
            /* 查找设备节点，由设备类节点指向的节点链查找 */
            else
            {
                __dev_node_t *pNode;

                pNode = pClassnode->nodelist;
                for (i = 0; pNode && i < idx; pNode = pNode->next, i ++)
                    ;
                if (!pNode)
                {
                    res = EPDK_FAIL;
                }
                else
                {
                    /* 返回找到的设备节点指针 */
                    dinfo->name = pNode->name;
                    dinfo->hnode = (__hdle)pNode;
                    dinfo->type = DEVFS_NODE_TYPE;
                    res = EPDK_OK;
                }
            }

            return res;
        }
        default:
            return EPDK_FAIL;
    }
}

static __dev_devop_t dmsdev_ops =
{
    dmsdev_Open,
    dmsdev_Close,
    dmsdev_Read,
    dmsdev_Write,
    dmsdev_Ioctrl
};

