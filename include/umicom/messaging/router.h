/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/router.h
 *
 * PURPOSE:
 *   Declare content-based routes from typed subscriptions to bounded channels with optional fan-out and route statistics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiMessageRouter UmiMessageRouter;

typedef struct UmiRouterStats {
    size_t routes;
    uint64_t messages;
    uint64_t deliveries;
    uint64_t unrouted;
} UmiRouterStats;

UmiStatus umi_message_router_create(size_t capacity,
                                    UmiMessageRouter **out_router);
void umi_message_router_destroy(UmiMessageRouter *router);
UmiStatus umi_message_router_add(UmiMessageRouter *router,
                                 const UmiSubscription *subscription,
                                 UmiChannel *destination);
UmiStatus umi_message_router_route(UmiMessageRouter *router,
                                   const UmiMessageEnvelope *message,
                                   size_t *out_deliveries);
UmiRouterStats umi_message_router_stats(const UmiMessageRouter *router);

#ifdef __cplusplus
}
#endif

#endif
