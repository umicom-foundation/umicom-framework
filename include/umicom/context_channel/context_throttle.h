/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_throttle.h
 *
 * PURPOSE:
 *   Bound publication rates without changing domain payload semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_THROTTLE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_THROTTLE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context throttle data shared with callers of this public contract.
 */
typedef struct UmiContextThrottle {
    uint32_t structure_size;
    char throttle_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char application_id[UMI_CONTEXT_VALUE_CAPACITY];
    char reason[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextThrottle;
/**
 * Initialise context throttle from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_throttle_init(UmiContextThrottle *state);
/**
 * Provide the context throttle set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_throttle_set_field(UmiContextThrottle *state,size_t field_index,const char *value);
/**
 * Provide the context throttle field operation used by this module and its client
 * applications.
 */
const char *umi_context_throttle_field(const UmiContextThrottle *state,size_t field_index);
/**
 * Provide the context throttle record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_throttle_record_success(UmiContextThrottle *state,uint64_t sequence);
/**
 * Provide the context throttle record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_throttle_record_failure(UmiContextThrottle *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context throttle satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_throttle_validate(const UmiContextThrottle *state);
/**
 * Provide the context throttle covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_throttle_covers_sequence(const UmiContextThrottle *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
