/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_backpressure.h
 *
 * PURPOSE:
 *   Expose subscriber backlog and delivery pressure for operational control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_BACKPRESSURE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_BACKPRESSURE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context backpressure data shared with callers of this public contract.
 */
typedef struct UmiContextBackpressure {
    uint32_t structure_size;
    char pressure_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char subscription_id[UMI_CONTEXT_VALUE_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextBackpressure;
/**
 * Initialise context backpressure from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_backpressure_init(UmiContextBackpressure *state);
/**
 * Provide the context backpressure set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_backpressure_set_field(UmiContextBackpressure *state,size_t field_index,const char *value);
/**
 * Provide the context backpressure field operation used by this module and its client
 * applications.
 */
const char *umi_context_backpressure_field(const UmiContextBackpressure *state,size_t field_index);
/**
 * Provide the context backpressure record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_backpressure_record_success(UmiContextBackpressure *state,uint64_t sequence);
/**
 * Provide the context backpressure record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_backpressure_record_failure(UmiContextBackpressure *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context backpressure satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_backpressure_validate(const UmiContextBackpressure *state);
/**
 * Provide the context backpressure covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_backpressure_covers_sequence(const UmiContextBackpressure *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
