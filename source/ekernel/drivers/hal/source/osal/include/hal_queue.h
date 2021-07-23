/*
 * ===========================================================================================
 *
 *       Filename:  hal_queue.h
 *
 *    Description:  header files to define osal timer for hal.
 *
 *        Version:  Melis3.0
 *         Create:  2019-11-25 20:50:36
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-27 18:00:56
 *
 * ===========================================================================================
 */

#ifndef SUNXI_HAL_MAILBOX_H
#define SUNXI_HAL_MAILBOX_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>
#include <stddef.h>
#include <stdint.h>

enum IPC_MAILBOX_CMD
{
    IPC_MAILBOX_CMD_GET_STATE,
    IPC_MAILBOX_CMD_NUMS,
};

typedef rt_mailbox_t hal_mailbox_t;
int hal_mailbox_create(const char *name, unsigned int size, hal_mailbox_t *mailbox);
int hal_mailbox_delete(hal_mailbox_t);
int hal_mailbox_send(hal_mailbox_t, unsigned int value);
int hal_mailbox_send_wait(hal_mailbox_t, unsigned int value, int timeout);
int hal_mailbox_recv(hal_mailbox_t, unsigned int *value, int timeout);
int hal_mailbox_control(hal_mailbox_t, int cmd, void *arg);

#ifdef __cplusplus
}
#endif
#endif
