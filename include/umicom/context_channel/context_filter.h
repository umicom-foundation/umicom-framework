/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_filter.h
 *
 * PURPOSE:
 *   Record deterministic filter evaluation evidence for routing decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_FILTER_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_FILTER_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context filter data shared with callers of this public contract.
 */
typedef struct UmiContextFilter {
    uint32_t structure_size;
    char evaluation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char filter_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char field_name[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextFilter;
/**
 * Initialise context filter from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_filter_init(UmiContextFilter *state);
/**
 * Provide the context filter set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_filter_set_field(UmiContextFilter *state,size_t field_index,const char *value);
/**
 * Provide the context filter field operation used by this module and its client
 * applications.
 */
const char *umi_context_filter_field(const UmiContextFilter *state,size_t field_index);
/**
 * Provide the context filter record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_filter_record_success(UmiContextFilter *state,uint64_t sequence);
/**
 * Provide the context filter record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_filter_record_failure(UmiContextFilter *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context filter satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_filter_validate(const UmiContextFilter *state);
/**
 * Provide the context filter covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_filter_covers_sequence(const UmiContextFilter *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
