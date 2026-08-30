/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/dispatcher.h
 *
 * PURPOSE:
 *   Declare the Integration Fabric dispatcher that validates schemas and invokes matching typed handlers with delivery statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_DISPATCHER_H
#define UMICOM_MESSAGING_DISPATCHER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/schema.h"
#include "umicom/messaging/subscription.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDispatcher UmiDispatcher;
typedef UmiStatus (*UmiMessageHandler)(const UmiMessageEnvelope *message,
                                       void *user_data);

typedef struct UmiDispatcherStats {
    size_t handlers;
    uint64_t dispatched;
    uint64_t delivered;
    uint64_t rejected;
    uint64_t failures;
} UmiDispatcherStats;

UmiStatus umi_dispatcher_create(UmiSchemaRegistry *schemas,
                                UmiDispatcher **out_dispatcher);
void umi_dispatcher_destroy(UmiDispatcher *dispatcher);
UmiStatus umi_dispatcher_subscribe(UmiDispatcher *dispatcher,
                                   const UmiSubscription *subscription,
                                   UmiMessageHandler handler,
                                   void *user_data,
                                   uint64_t *out_subscription_id);
UmiStatus umi_dispatcher_unsubscribe(UmiDispatcher *dispatcher,
                                     uint64_t subscription_id);
UmiStatus umi_dispatcher_dispatch(UmiDispatcher *dispatcher,
                                  const UmiMessageEnvelope *message,
                                  size_t *out_delivery_count);
UmiDispatcherStats umi_dispatcher_stats(const UmiDispatcher *dispatcher);

#ifdef __cplusplus
}
#endif

#endif
