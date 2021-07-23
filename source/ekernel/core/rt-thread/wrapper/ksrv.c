/*
 * ===========================================================================================
 *
 *       Filename:  ksrv.c
 *
 *    Description:  kernel service definition.
 *
 *        Version:  Melis3.0
 *         Create:  2018-04-24 11:10:23
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-05-08 20:34:27
 *
 * ===========================================================================================
 */
#include "ksrv.h"
#include <kapi.h>
#include <rthw.h>
#include <log.h>
#include <kconfig.h>
#include <string.h>
#include <stdio.h>
#include <boot/boot.h>
#include <arch.h>

static void kservice_maintask(void *p_arg);

static rt_mailbox_t app_msgq_l;
static rt_mailbox_t app_msgq_h;
static rt_mailbox_t ksrv_msgq;

extern boot_head_t kernel_param;
__s32 encrypt_para_init(void);
__ksrv_add_para g_ksrv_add_para = { 0 };
/*
**********************************************************************************************************************
*                                               KSRV_Init
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 ksrv_init(void)
{
    rt_thread_t ksrv_t;

    ksrv_msgq  = rt_mb_create("ksrv_mq", 16, 0);
    app_msgq_l = rt_mb_create("appmsgql_mq", 32, 0);
    app_msgq_h = rt_mb_create("appmsgqh_mq", 128, 0);

    if (!app_msgq_l || !app_msgq_h || !ksrv_msgq)
    {
        __err("create msgq failed!");
        return -1;
    }

    ksrv_t = rt_thread_create("kservice", kservice_maintask, RT_NULL, 0x4000, RT_TIMER_THREAD_PRIO - 2, 10);
    if (ksrv_t == RT_NULL)
    {
        __err("create kernel service task failure.");
        return -1;
    }
    rt_thread_startup(ksrv_t);

    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               KSRV_Exit
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 ksrv_exit(void)
{
    rt_mb_delete(ksrv_msgq);
    rt_mb_delete(app_msgq_l);
    rt_mb_delete(app_msgq_h);
    ksrv_msgq = RT_NULL;
    app_msgq_l = RT_NULL;
    app_msgq_h = RT_NULL;

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               kservice_maintask
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void kservice_maintask(void *p_arg)
{
    __epos_kmsg_t *msg;
    __u32 cpu_sr;
    rt_err_t err;

    while (1)
    {
        msg = RT_NULL;
        err = rt_mb_recv(ksrv_msgq, (rt_ubase_t *)&msg, RT_WAITING_FOREVER);
        if (err != RT_EOK)
        {
            __err("get message fail.");
            continue;
        }

        switch (msg->target)
        {
            case KMSG_TGT_INT_TAIL:                 /* 临时的特殊处理，未来想好的更好的办法再更新                   */
            {
                /* 由于msg是不需要释放的，为了避免后续的消息用相同的msg单元， 加上临界区，并copy以隔离              */
                /* 对于中断尾，由于中断可能来多次，消息时容许用相同的消息单元的，因为不需要释放，静态分配的         */
                /* 此处用tmpmsg将消息备份起来再使用，主要是怕前后的消息的消息内容有可能不一样（通常一样）           */
                __epos_kmsg_t tmpmsg;

                memset(&tmpmsg, 0x00, sizeof(tmpmsg));
                cpu_sr = rt_hw_interrupt_disable();
                tmpmsg = *msg;
                rt_hw_interrupt_enable(cpu_sr);

                (tmpmsg.l.cb)(tmpmsg.h.cb_u_arg, 0);
                continue;                           /* not need free buffer     */
            }

            case KMSG_TGT_CALLBACK:
                (msg->l.cb)(msg->h.cb_u_arg, 0);
                break;

            case KMSG_TGT_BROADCST:
            {
                switch (msg->message)
                {
                    case KMSG_VAL_PHONE_CMD:
                    {
                        __err("KMSG_VAL_PHONE_CMD, msg_data=%d", msg->l.para);
                        esKSRV_SendMsg(msg->l.para, KMSG_PRIO_HIGH);
                        break;
                    }

                    case KMSG_VAL_DEV_PLUGIN:
                    {
                        switch (msg->l.para)
                        {
                            case KMSG_VAL_DEV_USBD:
                                //__log("USB device plug in message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBD_PLUGIN, KMSG_PRIO_HIGH);
                                break;
                            case KMSG_VAL_DEV_USBH:
                                //__log("USB host plug in message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBH_PLUGIN, KMSG_PRIO_HIGH);
                                break;
                            case KMSG_VAL_DEV_SDMMC:
                                //__log("SDMMC card plug in message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_SDMMC_PLUGIN, KMSG_PRIO_HIGH);
                                break;
                            default:
                                break;
                        }
                    }
                    break;

                    case KMSG_VAL_DEV_PLUGOUT:
                    {
                        switch (msg->l.para)
                        {
                            case KMSG_VAL_DEV_USBD:
                                //__log("USB device plug out message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBD_PLUGOUT, KMSG_PRIO_HIGH);
                                break;
                            case KMSG_VAL_DEV_USBH:
                                //__log("USB host plug out message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBH_PLUGOUT, KMSG_PRIO_HIGH);
                                break;
                            case KMSG_VAL_DEV_SDMMC:
                                //__log("SDMMC card plug out message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_SDMMC_PLUGOUT, KMSG_PRIO_HIGH);
                                break;
                            default:
                                break;
                        }
                    }
                    break;

                    case KMSG_VAL_FS_PLUGIN:
                    {
                        int message;

                        message = KMSG_USR_SYSTEM_FS_PLUGIN |
                                  ((msg->l.para << 16) & KMSG_USR_SYSTEM_FS_PARA_MASK);
                        __log("partition [%c] plug in..", msg->l.para);
                        esKSRV_SendMsg(message, KMSG_PRIO_HIGH);
                    }
                    break;

                    case KMSG_VAL_FS_PLUGOUT:
                    {
                        int message;

                        message = KMSG_USR_SYSTEM_FS_PLUGOUT |
                                  ((msg->l.para << 16) & KMSG_USR_SYSTEM_FS_PARA_MASK);
                        __log("partition [%c] plug out..", msg->l.para);
                        esKSRV_SendMsg(message, KMSG_PRIO_HIGH);
                    }
                    break;

                    case KMSG_VAL_USBD_CONNECT:
                    {
                        switch (msg->l.para)
                        {
                            case KMSG_USR_SYSTEM_USBD_NOT_CONNECT:
                                //__log("USB device plug in message...");
                                esKSRV_SendMsg(KMSG_USR_SYSTEM_USBD_NOT_CONNECT, KMSG_PRIO_HIGH);
                                //__log("free __epos_kmsg_t");
                                break;

                            default:
                                //__log("free __epos_kmsg_t.");
                                break;
                        }
                    }
                    break;

                    default:
                        break;
                }
                break;
            }

            default:
                __err("msg->target = %d.", msg->target);
                break;
        }
        rt_free((void *)msg);
    }
}

__s32 esKSRV_SysInfo(void)
{
    rt_uint32_t total, used, max_used;

    esCLK_SysInfo("all");
    rt_memory_info(&total, &used, &max_used);

    printf("%14s"
           "%12s 0x%08x"
           "%12s 0x%08x"
           "%12s 0x%08x",
           "Mem info:", "Total:", total, "Used:", used, "Summit:", max_used);

    return 0;
}
/*
**********************************************************************************************************************
*                                               esKSRV_SendMsgEx
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 esKSRV_SendMsgEx(void *msg)
{
    __epos_kmsg_t *p_msg = (__epos_kmsg_t *)msg;

    if (p_msg == NULL || ksrv_msgq == NULL)
    {
        __err("wrong parameter.");
        return EPDK_FAIL;
    }

    if (rt_interrupt_get_nest())
    {
        return EPDK_FAIL;
    }

    switch (p_msg->target)
    {
        case KMSG_TGT_INT_TAIL:
            if (p_msg->prio)
            {
                __err("high prio not support!");
            }
            if (ksrv_msgq)
            {
                rt_mb_send(ksrv_msgq, (rt_uint32_t)msg);
            }
            return EPDK_OK;

        case KMSG_TGT_CALLBACK:
            if (p_msg->prio)
            {
                if (p_msg->l.cb)
                {
                    (p_msg->l.cb)(p_msg->h.cb_u_arg, 0);
                }
                rt_free(msg);
                return EPDK_OK;
            }
            if (ksrv_msgq)
            {
                rt_mb_send(ksrv_msgq, (rt_uint32_t)msg);
            }
            return EPDK_OK;
        case KMSG_TGT_BROADCST:
            if (ksrv_msgq)
            {
                rt_mb_send(ksrv_msgq, (rt_uint32_t)msg);
            }
            return EPDK_OK;
        default:
            if (ksrv_msgq)
            {
                rt_mb_send(ksrv_msgq, (rt_uint32_t)msg);
            }
            return EPDK_OK;
    }
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               esKSRV_GetMsg
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__u32 esKSRV_GetMsg(void)
{
    rt_err_t  err;
    __u32     ret;

    if (app_msgq_h == RT_NULL)
    {
        return 0;
    }
    if (app_msgq_l == RT_NULL)
    {
        return 0;
    }
    err = rt_mb_recv(app_msgq_h, (rt_ubase_t *)&ret, 0);
    if (err != RT_EOK)
    {
        err = rt_mb_recv(app_msgq_l, (rt_ubase_t *)&ret, 0);
        if (err != RT_EOK)
        {
            ret = 0;
        }
    }

    return ret;
}


/*
**********************************************************************************************************************
*                                               esKSRV_GetMsg
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__u32 esKSRV_GetLowMsg(void)
{
    rt_err_t  err;
    __u32     ret;

    if (app_msgq_l == RT_NULL)
    {
        return 0;
    }
    err = rt_mb_recv(app_msgq_l, (rt_ubase_t *)&ret, 0);
    if (err != RT_EOK)
    {
        ret = 0;
    }

    return ret;
}


/*
**********************************************************************************************************************
*                                               esKSRV_GetMsg
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__u32 esKSRV_GetHighMsg(void)
{
    rt_err_t  err;
    __u32     ret;

    if (app_msgq_h == RT_NULL)
    {
        return 0;
    }
    err = rt_mb_recv(app_msgq_h, (rt_ubase_t *)&ret, 0);
    if (err != RT_EOK)
    {
        ret = 0;
    }

    return ret;

}

/*
**********************************************************************************************************************
*                                               esKSRV_PutUsrMsg
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 esKSRV_SendMsg(__u32 msgid, __u32 prio)
{
    rt_uint32_t ret;

    if (app_msgq_h == RT_NULL)
    {
        return -1;
    }
    if (prio)
    {
        while (rt_mb_send(app_msgq_h, msgid) == -RT_EFULL)
        {
            __err("high message pool full!");
            rt_mb_recv(app_msgq_h, (void *)&ret, 0);
        }
    }
    else
    {
        while (rt_mb_send(app_msgq_l, msgid) == -RT_EFULL)
        {
            __err("low message pool full!");
            rt_mb_recv(app_msgq_l, (void *)&ret, 0);
        }
    }
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               esKSRV_GetMsg
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__u32 esKSRV_GetVersion(__epdk_ver_type_t type)
{
    __u32 ver = 0;

    switch (type)
    {
        case EPDK_VER:
            ver = 0x2000000;
            break;
        case EPDK_VER_OS:
            ver = 0;
            break;
        default:
            break;
    }
    return ver;
}
