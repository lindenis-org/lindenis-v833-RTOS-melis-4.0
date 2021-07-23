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
* update  : kevin.z.m 2010-9-7 17:05, clean code
**********************************************************************************************************************
*/
#include "dev.h"
#include <log.h>
#include <port.h>
#include <kapi.h>

/*
**********************************************************************************************************************
*                                     esDEV_Open
*
* Description: module init function, this function will be called by system module management: MInstall,
*               user will never call it.
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/
__hdle esDEV_Open(__hdle hNode, __u32 Mode)
{
    __dev_dev_t      *pDev;
    __hdle            hDev;
    __dev_node_t     *pNode = (__dev_node_t *)hNode;

    if (pNode->status == DEV_STAT_INACTIVE)
    {
        __wrn("Device has been killed when unreg!");
        return NULL;
    }

    if (pNode->opentimes == 0)
    {
        hDev = pNode->DevOp.Open(pNode->pOpenArg, Mode);/* 打开设备                                                     */
        if (hDev == NULL)
        {
            __wrn("dev cannot be open!");
            return NULL;
        }
        pNode->hDev = hDev;                         /* 保存设备句柄                                                 */
    }                                               /* 分配设备管理句柄空间                                         */

    pDev = (__dev_dev_t *)k_malloc(sizeof(__dev_dev_t));
    if (pDev == NULL)
    {
        __err("alloc structure failure");
        return NULL;
    }

    pDev->devnode = pNode;                          /* 记录node指针                                                 */
    pDev->DevOp   = pNode->DevOp;                     /* 设备操作入口，直接从__dev_node_t里copy，以方便使用           */
    pDev->hDev    = pNode->hDev;                       /* 设备句柄，直接从__dev_node_t里copy，以方便使用               */

    pNode->opentimes ++;                             /* 设备节点打开次数减1                                          */

    return pDev;                            /* 返回设备管理句柄                                             */
}
/*
**********************************************************************************************************************
*                                     esDEV_Close
*
* Description: 关闭设备
*
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/
__s32  esDEV_Close(__hdle hDev)
{
    __dev_dev_t  *pDev  = (__dev_dev_t *)hDev;
    __dev_node_t *pNode = pDev->devnode;
    __u8          err;

    if (pNode->opentimes)
    {
        pNode->opentimes--;    /* 设备节点打开次数减1                                          */
    }

    if (pNode->opentimes == 0)                      /* 如果设备节点已经没有人再使用                                 */
    {
        /* 如果设备节点没有用户使用，同时其又是死节点，做反注册的操作 */
        if (pNode->status == DEV_STAT_INACTIVE)
        {
            __dev_node_t *p, **pp = &(pNode->classnode->nodelist);

            for (p = *pp; p && (pNode != p); pp = &(p->next), p = p->next);

            if (!p)
            {
                __wrn("BUG when close dev: try to destroy a devnode not exsit in node list!");
                return EPDK_FAIL;
            }

            *pp = pNode->next;

            /* 释放设备节点占用的空间               */
            esKRNL_SemDel(pNode->sem, 0, &err);
            k_free((void *)pNode);
        }
        else                                        /* 否则，已经没有用户打开此设备节点，同时此设备节点不是死节点   */
        {
            if (pDev->DevOp.Close(pNode->hDev) == EPDK_FAIL) /* 关闭设备句柄                                         */
            {
                __err("close device[%s] fail!", pNode->name);
            }
        }
    }

    k_free((void *)pDev);              /* 释放设备句柄占用的空间                                       */

    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                     esDEV_Read
*
* Description: 读
*
* Arguments  :  pdata       需要读出的数据指针
*               size        块的大小
*               n           块数
*               hDev        设备句柄
*
* Returns    : 实际读出的块数
*
**********************************************************************************************************************
*/
__u32  esDEV_Read(void *pdata, __u32 size, __u32 n, __hdle hDev)
{
    __dev_node_t *pNode = ((__dev_dev_t *)hDev)->devnode;

    if (pNode->status == DEV_STAT_INACTIVE)
    {
        __wrn("Device has been killed when unreg!");
        return 0;
    }

    return (((__dev_dev_t *)hDev)->DevOp.Read)(pdata, size, n, ((__dev_dev_t *)hDev)->hDev);
}


/*
**********************************************************************************************************************
*                                     esDEV_Write
*
* Description: 写
*
* Arguments  :  pdata       需要写入的数据指针
*               size        块的大小
*               n           块数
*               hDev        设备句柄
*
* Returns    : 实际写入的块数
*
**********************************************************************************************************************
*/
__u32  esDEV_Write(const void *pdata, __u32 size, __u32 n, __hdle hDev)
{
    __dev_node_t *pNode = ((__dev_dev_t *)hDev)->devnode;


    if (pNode->status == DEV_STAT_INACTIVE)
    {
        __wrn("Device has been killed when unreg!");
        return 0;
    }

    return (((__dev_dev_t *)hDev)->DevOp.Write)(pdata, size, n, ((__dev_dev_t *)hDev)->hDev);
}


/*
**********************************************************************************************************************
*                                     esDEV_Ioctrl
*
* Description: 设备控制
*
* Arguments  : hDev         设备句柄
*              cmd          命令
*              aux          参数
*              pbuffer      数据buffer
*
* Returns    : device defined
*
**********************************************************************************************************************
*/
__s32  esDEV_Ioctl(__hdle hDev, __u32 cmd, __s32 aux, void *pbuffer)
{
    __dev_node_t *pNode = ((__dev_dev_t *)hDev)->devnode;

    if (pNode->status == DEV_STAT_INACTIVE)
    {
        __wrn("Device has been killed when unreg!");
        return EPDK_FAIL;
    }

    if (IS_DEVIOCSYS(cmd))
    {
        switch (cmd)
        {
            case DEV_IOC_SYS_GET_CLSNAME:
                *((int *)pbuffer) = (int)pNode->classnode->name;
                return EPDK_OK;

            case DEV_IOC_SYS_GET_DEVNAME:
                *((int *)pbuffer) = (int)pNode->name;
                return EPDK_OK;
            case DEV_IOC_SYS_GET_ATTRIB:
                *((int *)pbuffer) = pNode->attrib;
                return EPDK_OK;
            case DEV_IOC_SYS_GET_LETTER:
                *((char *)pbuffer) = pNode->pletter;
                return EPDK_OK;
            case DEV_IOC_SYS_GET_OPENARGS:
                *((int *)pbuffer) = (int)pNode->pOpenArg;
                return EPDK_OK;
            default:
                return EPDK_FAIL;
        }
    }
    else
        return (((__dev_dev_t *)hDev)->DevOp.Ioctl)
               (((__dev_dev_t *)hDev)->hDev, cmd, aux, pbuffer);
}

