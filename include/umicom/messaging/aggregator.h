/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/aggregator.h
 *
 * PURPOSE:
 *   Declare correlation-based message aggregation that owns partial groups and emits a complete ordered group at the required count.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiMessageAggregator UmiMessageAggregator;

typedef struct UmiAggregateView {
    uint64_t correlation_id;
    const UmiOwnedMessage *messages;
    size_t count;
} UmiAggregateView;

UmiStatus umi_message_aggregator_create(size_t group_capacity,
                                        size_t messages_per_group,
                                        UmiMessageAggregator **out_aggregator);
void umi_message_aggregator_destroy(UmiMessageAggregator *aggregator);
UmiStatus umi_message_aggregator_add(UmiMessageAggregator *aggregator,
                                     const UmiMessageEnvelope *message,
                                     UmiAggregateView *out_complete_group);
UmiStatus umi_message_aggregator_release(UmiMessageAggregator *aggregator,
                                         uint64_t correlation_id);
size_t umi_message_aggregator_group_count(
    const UmiMessageAggregator *aggregator);

#ifdef __cplusplus
}
#endif

#endif
