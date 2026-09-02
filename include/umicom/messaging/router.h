/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/router.h
 *
 * PURPOSE:
 *   Declare content-based routes from typed subscriptions to bounded channels with optional fan-out and route statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_ROUTER_H
#define UMICOM_MESSAGING_ROUTER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/channel.h"
#include "umicom/messaging/subscription.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the message router data shared with callers of this public contract.
 */
typedef struct UmiMessageRouter UmiMessageRouter;

/**
 * Represent the router stats data shared with callers of this public contract.
 */
typedef struct UmiRouterStats {
    size_t routes;
    uint64_t messages;
    uint64_t deliveries;
    uint64_t unrouted;
} UmiRouterStats;

/**
 * Initialise message router from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_message_router_create(size_t capacity,
                                    UmiMessageRouter **out_router);
/**
 * Release or reset state held by message router so the same storage can be reused safely.
 */
void umi_message_router_destroy(UmiMessageRouter *router);
/**
 * Add message router only after its inputs and available capacity have been checked.
 */
UmiStatus umi_message_router_add(UmiMessageRouter *router,
                                 const UmiSubscription *subscription,
                                 UmiChannel *destination);
/**
 * Provide the message router route operation used by this module and its client
 * applications.
 */
UmiStatus umi_message_router_route(UmiMessageRouter *router,
                                   const UmiMessageEnvelope *message,
                                   size_t *out_deliveries);
/**
 * Provide the message router stats operation used by this module and its client
 * applications.
 */
UmiRouterStats umi_message_router_stats(const UmiMessageRouter *router);

#ifdef __cplusplus
}
#endif

#endif
