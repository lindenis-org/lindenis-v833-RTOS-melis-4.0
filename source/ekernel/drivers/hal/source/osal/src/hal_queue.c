/*
 * ===========================================================================================
 *
 *       Filename:  hal_mailbox.c
 *
 *    Description:  mbox impl.for hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-27 17:32:09
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-27 18:02:12
 *
 * ===========================================================================================
 */

#include <hal_queue.h>
#include <rtdef.h>
#include <log.h>

int hal_mailbox_create(const char *name, unsigned int size, hal_mailbox_t *mailbox)
{
    if (mailbox == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return -1;
    }

    *mailbox = rt_mb_create("hal_layer", size, RT_IPC_FLAG_FIFO);
    if (*mailbox == RT_NULL)
    {
        __err("fatal error, create sys semphore failure.");
        return -1;
    }

    return 0;
}

int hal_mailbox_delete(hal_mailbox_t mailbox)
{
    if (mailbox == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return -1;
    }

    rt_mb_delete(mailbox);
    return 0;
}

int hal_mailbox_send(hal_mailbox_t mailbox, unsigned int value)
{
    rt_err_t ret;

    if (mailbox == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return -1;
    }

    ret = rt_mb_send(mailbox, value);
    if (ret != RT_EOK)
    {
        // timeout.
        return -2;
    }

    return 0;
}

int hal_mailbox_send_wait(hal_mailbox_t mailbox, unsigned int value, int timeout)
{
    rt_err_t ret;

    if (mailbox == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return -1;
    }

    ret = rt_mb_send_wait(mailbox, value, timeout);
    if (ret != RT_EOK)
    {
        // timeout.
        return -2;
    }

    return 0;
}

int hal_mailbox_recv(hal_mailbox_t mailbox, unsigned int *value, int timeout)
{
    rt_err_t ret;

    if (mailbox == NULL || value == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return -1;
    }

    ret = rt_mb_recv(mailbox, (rt_ubase_t *)value, timeout);
    if (ret != RT_EOK)
    {
        // timeout.
        return -2;
    }

    return 0;
}

int hal_mailbox_control(hal_mailbox_t mailbox, int cmd, void *arg)
{
    rt_err_t ret = -1;

    if (mailbox == NULL || arg == NULL)
    {
        __err("fatal error, parameter is invalid.");
        return -1;
    }

    switch (cmd)
    {
        case IPC_MAILBOX_CMD_GET_STATE:
            ret = rt_mb_control(mailbox, RT_IPC_CMD_GET_STATE, arg);
            if (ret != RT_EOK)
            {
                return -2;
            }
            break;
        default:
            ret = -1;
            break;
    }
    return ret;
}
