/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Network Management API public header
 */

#ifndef __NET_MGMT_H__
#define __NET_MGMT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Network Management
 * @defgroup net_mgmt Network Management
 * @{
 */

#include <misc/__assert.h>
#include <net/net_core.h>

struct net_if;

/**
 * @brief NET MGMT event mask basics, normalizing parts of bit fields
 */
#define NET_MGMT_EVENT_MASK     0x80000000
#define NET_MGMT_ON_IFACE_MASK      0x40000000
#define NET_MGMT_LAYER_MASK     0x30000000
#define NET_MGMT_SYNC_EVENT_MASK    0x08000000
#define NET_MGMT_LAYER_CODE_MASK    0x07FF0000
#define NET_MGMT_COMMAND_MASK       0x0000FFFF

#define NET_MGMT_EVENT_BIT      BIT(31)
#define NET_MGMT_IFACE_BIT      BIT(30)
#define NET_MGMT_SYNC_EVENT_BIT     BIT(27)

#define NET_MGMT_LAYER(_layer)      (_layer << 28)
#define NET_MGMT_LAYER_CODE(_code)  (_code << 16)

#define NET_MGMT_EVENT(mgmt_request)        \
    (mgmt_request & NET_MGMT_EVENT_MASK)

#define NET_MGMT_ON_IFACE(mgmt_request)     \
    (mgmt_request & NET_MGMT_ON_IFACE_MASK)

#define NET_MGMT_EVENT_SYNCHRONOUS(mgmt_request)    \
    (mgmt_request & NET_MGMT_SYNC_EVENT_MASK)

#define NET_MGMT_GET_LAYER(mgmt_request)    \
    ((mgmt_request & NET_MGMT_LAYER_MASK) >> 28)

#define NET_MGMT_GET_LAYER_CODE(mgmt_request)   \
    ((mgmt_request & NET_MGMT_LAYER_CODE_MASK) >> 16)

#define NET_MGMT_GET_COMMAND(mgmt_request)  \
    (mgmt_request & NET_MGMT_COMMAND_MASK)


/* Useful generic definitions */
#define NET_MGMT_LAYER_L1       1
#define NET_MGMT_LAYER_L2       2
#define NET_MGMT_LAYER_L3       3

#include <net/net_event.h>


/**
 * @typedef net_mgmt_request_handler_t
 * @brief Signature which all Net MGMT request handler need to follow
 * @param mgmt_request The exact request value the handler is being called
 *        through
 * @param iface A valid pointer on struct net_if if the request is meant
 *        to be tight to a network interface. NULL otherwise.
 * @param data A valid pointer on a data understood by the handler.
 *        NULL otherwise.
 * @param len Length in byte of the memory pointed by data.
 */
typedef int (*net_mgmt_request_handler_t)(u32_t mgmt_request,
        struct net_if *iface,
        void *data, size_t len);

#define net_mgmt(_mgmt_request, _iface, _data, _len)            \
    net_mgmt_##_mgmt_request(_mgmt_request, _iface, _data, _len)

#define NET_MGMT_DEFINE_REQUEST_HANDLER(_mgmt_request)          \
    extern int net_mgmt_##_mgmt_request(u32_t mgmt_request, \
                                        struct net_if *iface,   \
                                        void *data, size_t len)

#define NET_MGMT_REGISTER_REQUEST_HANDLER(_mgmt_request, _func) \
    FUNC_ALIAS(_func, net_mgmt_##_mgmt_request, int)

struct net_mgmt_event_callback;

/**
 * @typedef net_mgmt_event_handler_t
 * @brief Define the user's callback handler function signature
 * @param "struct net_mgmt_event_callback *cb"
 *        Original struct net_mgmt_event_callback owning this handler.
 * @param "u32_t mgmt_event" The network event being notified.
 * @param "struct net_if *iface" A pointer on a struct net_if to which the
 *        the event belongs to, if it's an event on an iface. NULL otherwise.
 */
typedef void (*net_mgmt_event_handler_t)(struct net_mgmt_event_callback *cb,
        u32_t mgmt_event,
        struct net_if *iface);

/**
 * @brief Network Management event callback structure
 * Used to register a callback into the network management event part, in order
 * to let the owner of this struct to get network event notification based on
 * given event mask.
 */
struct net_mgmt_event_callback
{
    /** Meant to be used internally, to insert the callback into a list.
     * So nobody should mess with it.
     */
    sys_snode_t node;

    union
    {
        /** Actual callback function being used to notify the owner
         */
        net_mgmt_event_handler_t handler;
        /** Semaphore meant to be used internaly for the synchronous
         * net_mgmt_event_wait() function.
         */
        struct k_sem *sync_call;
    };

    /** A mask of network events on which the above handler should be
     * called in case those events come. Such mask can be modified
     * whenever necessary by the owner, and thus will affect the handler
     * being called or not.
     */
    union
    {
        /** A mask of network events on which the above handler should
         * be called in case those events come. Such mask can be
         * modified whenever necessary by the owner, and thus will
         * affect the handler being called or not.
         */
        u32_t event_mask;
        /** Internal place holder when a synchronous event wait is
         * successfully unlocked on a event.
         */
        u32_t raised_event;
    };
};

#ifdef CONFIG_NET_MGMT_EVENT
/**
 * @brief Helper to initialize a struct net_mgmt_event_callback properly
 * @param cb A valid application's callback structure pointer.
 * @param handler A valid handler function pointer.
 * @param mgmt_event_mask A mask of relevant events for the handler
 */
static inline
void net_mgmt_init_event_callback(struct net_mgmt_event_callback *cb,
                                  net_mgmt_event_handler_t handler,
                                  u32_t mgmt_event_mask)
{
    __ASSERT(cb, "Callback pointer should not be NULL");
    __ASSERT(handler, "Handler pointer should not be NULL");
    cb->handler = handler;
    cb->event_mask = mgmt_event_mask;
};

/**
 * @brief Add a user callback
 * @param cb A valid pointer on user's callback to add.
 */
void net_mgmt_add_event_callback(struct net_mgmt_event_callback *cb);

/**
 * @brief Delete a user callback
 * @param cb A valid pointer on user's callback to delete.
 */
void net_mgmt_del_event_callback(struct net_mgmt_event_callback *cb);

/**
 * @brief Used by the system to notify an event.
 * @param mgmt_event The actual network event code to notify
 * @param iface a valid pointer on a struct net_if if only the event is
 *        based on an iface. NULL otherwise.
 */
void net_mgmt_event_notify(u32_t mgmt_event, struct net_if *iface);

/**
 * @brief Used to wait synchronously on an event mask
 * @param mgmt_event_mask A mask of relevant events to wait on.
 * @param raised_event a pointer on a u32_t to get which event from
 *        the mask generated the event. Can be NULL if the caller is not
 *        interested in that information.
 * @param iface a pointer on a place holder for the iface on which the
 *        event has originated from. This is valid if only the event mask
 *        has bit NET_MGMT_IFACE_BIT set relevantly, depending on events
 *        the caller wants to listen to.
 * @param timeout a delay in milliseconds. K_FOREVER can be used to wait
 *        indefinitely.
 *
 * @return 0 on success, a negative error code otherwise. -ETIMEDOUT will
 *         be specifically returned if the timeout kick-in instead of an
 *         actual event.
 */
int net_mgmt_event_wait(u32_t mgmt_event_mask,
                        u32_t *raised_event,
                        struct net_if **iface,
                        int timeout);

/**
 * @brief Used to wait synchronously on an event mask for a specific iface
 * @param iface a pointer on a valid network interface to listen event to
 * @param mgmt_event_mask A mask of relevant events to wait on. Listened
 *        to events should be relevant to iface events and thus have the bit
 *        NET_MGMT_IFACE_BIT set.
 * @param raised_event a pointer on a u32_t to get which event from
 *        the mask generated the event. Can be NULL if the caller is not
 *        interested in that information.
 * @param timeout a delay in milliseconds. K_FOREVER can be used to wait
 *        indefinitely.
 *
 * @return 0 on success, a negative error code otherwise. -ETIMEDOUT will
 *         be specifically returned if the timeout kick-in instead of an
 *         actual event.
 */
int net_mgmt_event_wait_on_iface(struct net_if *iface,
                                 u32_t mgmt_event_mask,
                                 u32_t *raised_event,
                                 int timeout);

/**
 * @brief Used by the core of the network stack to initialize the network
 *        event processing.
 */
void net_mgmt_event_init(void);
#else
#define net_mgmt_init_event_callback(...)
#define net_mgmt_add_event_callback(...)
#define net_mgmt_event_notify(...)
#define net_mgmt_event_init(...)

static inline int net_mgmt_event_wait(u32_t mgmt_event_mask,
                                      u32_t *raised_event,
                                      struct net_if **iface,
                                      int timeout)
{
    return 0;
}

static inline int net_mgmt_event_wait_on_iface(struct net_if *iface,
        u32_t mgmt_event_mask,
        u32_t *raised_event,
        int timeout)
{
    return 0;
}

#endif /* CONFIG_NET_MGMT_EVENT */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __NET_MGMT_H__ */
