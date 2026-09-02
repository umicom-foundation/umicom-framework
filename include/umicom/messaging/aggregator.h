/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/aggregator.h
 *
 * PURPOSE:
 *   Declare correlation-based message aggregation that owns partial groups and emits a complete ordered group at the required count.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_AGGREGATOR_H
#define UMICOM_MESSAGING_AGGREGATOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the message aggregator data shared with callers of this public contract.
 */
typedef struct UmiMessageAggregator UmiMessageAggregator;

/**
 * Represent the aggregate view data shared with callers of this public contract.
 */
typedef struct UmiAggregateView {
    uint64_t correlation_id;
    const UmiOwnedMessage *messages;
    size_t count;
} UmiAggregateView;

/**
 * Initialise message aggregator from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_message_aggregator_create(size_t group_capacity,
                                        size_t messages_per_group,
                                        UmiMessageAggregator **out_aggregator);
/**
 * Release or reset state held by message aggregator so the same storage can be reused
 * safely.
 */
void umi_message_aggregator_destroy(UmiMessageAggregator *aggregator);
/**
 * Add message aggregator only after its inputs and available capacity have been checked.
 */
UmiStatus umi_message_aggregator_add(UmiMessageAggregator *aggregator,
                                     const UmiMessageEnvelope *message,
                                     UmiAggregateView *out_complete_group);
/**
 * Release or reset state held by message aggregator so the same storage can be reused
 * safely.
 */
UmiStatus umi_message_aggregator_release(UmiMessageAggregator *aggregator,
                                         uint64_t correlation_id);
/**
 * Return the number of records represented by message aggregator group without changing
 * their state.
 */
size_t umi_message_aggregator_group_count(
    const UmiMessageAggregator *aggregator);

#ifdef __cplusplus
}
#endif

#endif
