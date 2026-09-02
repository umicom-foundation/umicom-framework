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

/**
 * Represent the dispatcher data shared with callers of this public contract.
 */
typedef struct UmiDispatcher UmiDispatcher;
typedef UmiStatus (*UmiMessageHandler)(const UmiMessageEnvelope *message,
                                       void *user_data);

/**
 * Represent the dispatcher stats data shared with callers of this public contract.
 */
typedef struct UmiDispatcherStats {
    size_t handlers;
    uint64_t dispatched;
    uint64_t delivered;
    uint64_t rejected;
    uint64_t failures;
} UmiDispatcherStats;

/**
 * Initialise dispatcher from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_dispatcher_create(UmiSchemaRegistry *schemas,
                                UmiDispatcher **out_dispatcher);
/**
 * Release or reset state held by dispatcher so the same storage can be reused safely.
 */
void umi_dispatcher_destroy(UmiDispatcher *dispatcher);
/**
 * Provide the dispatcher subscribe operation used by this module and its client
 * applications.
 */
UmiStatus umi_dispatcher_subscribe(UmiDispatcher *dispatcher,
                                   const UmiSubscription *subscription,
                                   UmiMessageHandler handler,
                                   void *user_data,
                                   uint64_t *out_subscription_id);
/**
 * Provide the dispatcher unsubscribe operation used by this module and its client
 * applications.
 */
UmiStatus umi_dispatcher_unsubscribe(UmiDispatcher *dispatcher,
                                     uint64_t subscription_id);
/**
 * Perform dispatcher through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_dispatcher_dispatch(UmiDispatcher *dispatcher,
                                  const UmiMessageEnvelope *message,
                                  size_t *out_delivery_count);
/**
 * Provide the dispatcher stats operation used by this module and its client applications.
 */
UmiDispatcherStats umi_dispatcher_stats(const UmiDispatcher *dispatcher);

#ifdef __cplusplus
}
#endif

#endif
