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
* update  : kevin.z.m, 2010-9-7 17:08, clear code.
**********************************************************************************************************************
*/
#include "dev.h"
#include <kapi.h>
#include <string.h>
#include <port.h>
#include <log.h>

extern __hdle    pDevSem;            /* for lock parts table */

__dev_classnode_t devclass_nop = {.name = DEV_CLASS_NOP, .nodetype = NODETYPE_CLASS,};

/*
**********************************************************************************************************************
*                                     SetDevAttr
*
* Description: set dev node attribute.
*
* Arguments  : class name
*              dev name
*              devnode
*
* Returns    : always EPDK_OK
**********************************************************************************************************************
*/
__s32 SetDevAttr(char *classname, char *devname, __dev_node_t *devnode)
{
    devnode->attrib = 0;

    if (!strcmp(classname, DEV_CLASS_DMS))
    {
        devnode->attrib |= DEV_NODE_ATTR_CTL
                           | DEV_NODE_ATTR_PART
                           | DEV_NODE_ATTR_FS
                           | DEV_NODE_ATTR_SYNMNT;
        devnode->pletter = PART_LETTER_DMS;
    }
    else if (!strcmp(classname, DEV_CLASS_DISK))
    {
        devnode->attrib |= DEV_NODE_ATTR_RD
                           | DEV_NODE_ATTR_WR
                           | DEV_NODE_ATTR_BLK
                           | DEV_NODE_ATTR_PART
                           | DEV_NODE_ATTR_FS;
        devnode->pletter = PART_LETTER_FREE;

        if (!strcmp(devname, DEV_NAME_RAMDISK))
        {
            devnode->pletter = PART_LETTER_RAMDISK;
            devnode->attrib |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_ROOTFS))
        {
            devnode->pletter = PART_LETTER_ROOTFS;
            devnode->attrib |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_SYSDATAFS))
        {
            devnode->pletter = PART_LETTER_SYSDATA;
            devnode->attrib |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_SYSBOOTFS))
        {
            devnode->pletter = PART_LETTER_SYSBOOT;
            devnode->attrib |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_BOOTFS))
        {
            devnode->pletter = PART_LETTER_BOOTFS;
            devnode->attrib |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strncmp(devname, DEV_NAME_USERDISK, strlen(DEV_NAME_USERDISK)))
        {

            /* 方案用户自定义分区，分区名格式为"USERDISKxx",
             * 其中xx表示00~99，用于区分各方案用户的分区名.
             * 方案用户自定义分区在W~U范围内根据注册顺序倒序(W~U)分配.
             */
            devnode->pletter = PART_LETTER_USER;
            devnode->attrib |= DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_SDCARD0) ||
                 !strcmp(devname, DEV_NAME_SDCARD1))
        {
            devnode->attrib |= DEV_NODE_ATTR_MOVABLE
                               | DEV_NODE_ATTR_USBSLOT
                               | DEV_NODE_ATTR_SYNMNT;
        }
        else if (!strcmp(devname, DEV_NAME_SCSI_DISK_00))
        {
            devnode->attrib |= DEV_NODE_ATTR_MOVABLE
                               | DEV_NODE_ATTR_USBSLOT;
        }
    }
    else
    {
        devnode->attrib  |= DEV_NODE_ATTR_RD
                            | DEV_NODE_ATTR_WR
                            | DEV_NODE_ATTR_CTL
                            | DEV_NODE_ATTR_SYNMNT;
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                     esDEV_DevReg
*
* Description: module init function, this function will be called by system module management: MInstall,
*               user will never call it.
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/
__hdle esDEV_DevReg(const char *classname, const char *name, const __dev_devop_t *pDevOp, void *pOpenArg)
{
    __u8               newclassflag = 0;
    __hdle             res;
    __dev_node_t      *devnode;
    __dev_node_t      *predevnode;
    __u8               err;

    __dev_classnode_t *classnode    = &devclass_nop;
    __dev_classnode_t *preclassnode = &devclass_nop;

    esKRNL_SemPend(pDevSem, 0, &err);
    if (err)
    {
        __err("pend semphore faiure.");
        return NULL;
    }

    /* step 1: 寻找class类型名，如果没有找到，则新建一个类型名                     */
    while (classnode)
    {
        if (strcmp(classname, classnode->name) == 0)
        {
            break;
        }
        preclassnode = classnode;
        classnode = classnode->next;
    }
    if (classnode == 0)
    {
        /*申请空间                              */
        classnode = k_malloc(sizeof(__dev_classnode_t) + strlen(classname) + 1/*'/0'*/);
        if (classnode == 0)
        {
            __wrn("k_malloc fail!");
            res = NULL;
            goto out;
        }
        /* 将classnode挂接到list中              */
        preclassnode->next = classnode;
        classnode->next    = 0;

        /* copy name into classnode             */
        classnode->nodetype = NODETYPE_CLASS;
        classnode->name = (char *)((__u32)classnode + sizeof(__dev_classnode_t));
        strcpy(classnode->name, classname);

        /* nodelist is empty                    */
        classnode->nodelist = 0;

        /* new class is created                 */
        newclassflag = 1;
    }
    /* step 2: 设备管理list里是否有相同的设备名，如果有，则返回错误                                                 */
    devnode = classnode->nodelist;
    predevnode = 0;

    while (devnode)
    {
        if (strcmp(devnode->name, name) == 0)
        {
            break;
        }
        predevnode = devnode;
        devnode = devnode->next;
    }
    if (devnode != 0)
    {
        __wrn("dev is already registered, or name conflict!");
        res = NULL;
        goto out;
    }

    /* step 3: 根据sizeofdrv创建设备节点                                                                            */
    devnode = k_malloc(sizeof(__dev_node_t) + strlen(name) + 1/*'/0'*/);
    if (devnode == 0)
    {
        __wrn("k_malloc fail!");
        if (newclassflag)       /* 如果class为新建，释放，并脱离class list */
        {
            k_free(classnode);
            preclassnode->next = 0;
        }
        res = NULL;
        goto out;
    }

    //记录设备名
    devnode->name = (char *)((__u32)devnode + sizeof(__dev_node_t));
    strcpy(devnode->name, name);
    //下一个为空
    devnode->next = 0;
    //记录class
    devnode->classnode = classnode;
    //打开次数为0
    devnode->opentimes = 0;
    //活节点
    devnode->status = DEV_STAT_ACTIVE;
    //保存device操作
    devnode->DevOp = *pDevOp;
    /* save open arg                                            */
    devnode->pOpenArg = pOpenArg;

    devnode->hDev = 0;

    devnode->sem = esKRNL_SemCreate(1);
    if (devnode->sem == NULL)
    {
        __err("create sem. failure");
        res = NULL;
        goto out;
    }

    /* 挂接到list中(link to rear of the list                    */
    if (predevnode == 0)
    {
        classnode->nodelist = devnode;
    }
    else
    {
        predevnode->next = devnode;
    }

    __inf("device \"%s\\%s\" is setup.", classname, name);

    /* step 4: 设置设备节点的属性 */
    SetDevAttr((char *)classname, (char *)name, devnode);

    /* step 5: 尝试注册分区和文件系统                           */
    if (devnode->attrib & DEV_NODE_ATTR_PART)
    {
        esFSYS_mntparts((__hdle)devnode);
    }

    res = (__hdle)devnode;

out:
    esKRNL_SemPost(pDevSem);
    return res;
}

/*
**********************************************************************************************************************
*                                     esDEV_DevUnreg
*
* Description: module init function, this function will be called by system module management: MInstall,
*               user will never call it.
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*              esle return EPDK_FAIL
**********************************************************************************************************************
*/

__s32  esDEV_DevUnreg(__hdle hNode)
{
    __s32  res;
    __dev_node_t  *pNode = (__dev_node_t *)hNode;
    __u8           err;

    esKRNL_SemPend(pDevSem, 0, &err);
    if (err)
    {
        __err("create semphore faile");
        return EPDK_FAIL;
    }

    /* 通知文件系统                                                                 */
    esFSYS_umntparts(hNode, 1);

    if (pNode->opentimes == 0)                      /* 如果node没有被任何用户使用，删除node，释放空间               */
    {
        __dev_node_t *p, **pp = &(pNode->classnode->nodelist);

        for (p = *pp; p && (pNode != p); pp = &(p->next), p = p->next);

        if (!p)
        {
            __wrn("BUG when unres dev: try to destroy a devnode not exsit in node list!");
            res = EPDK_FAIL;
            goto out;
        }
        *pp = pNode->next;

        esKRNL_SemDel(pNode->sem, 0, &err);
        k_free((void *)pNode); /* 释放node空间                                                 */
        res = EPDK_OK;
        goto out;
    }
    else                                            /* 否则，需要将node的状态设置为死node                           */
    {
        pNode->DevOp.Close(pNode->hDev);            /* 关闭设备句柄                                                 */
        pNode->status = DEV_STAT_INACTIVE;          /* 置设备节点为死节点                                           */
    }

    res = EPDK_OK;

out:
    esKRNL_SemPost(pDevSem);
    return res;
}

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
__s32 esDEV_Lock(__hdle hNode)
{
    esKRNL_SemPend(((__dev_node_t *)hNode)->sem, 0, NULL);
    return EPDK_OK;
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
__s32  esDEV_Unlock(__hdle hNode)
{
    esKRNL_SemPost(((__dev_node_t *)hNode)->sem);
    return EPDK_OK;
}

