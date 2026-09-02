/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_conflict.h
 *
 * PURPOSE:
 *   Represent incompatible concurrent context updates explicitly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_CONFLICT_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_CONFLICT_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context conflict data shared with callers of this public contract.
 */
typedef struct UmiContextConflict {
    uint32_t structure_size;
    char conflict_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char local_schema[UMI_CONTEXT_VALUE_CAPACITY];
    char remote_schema[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextConflict;
/**
 * Initialise context conflict from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_conflict_init(UmiContextConflict *state);
/**
 * Provide the context conflict set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_conflict_set_field(UmiContextConflict *state,size_t field_index,const char *value);
/**
 * Provide the context conflict field operation used by this module and its client
 * applications.
 */
const char *umi_context_conflict_field(const UmiContextConflict *state,size_t field_index);
/**
 * Provide the context conflict record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_conflict_record_success(UmiContextConflict *state,uint64_t sequence);
/**
 * Provide the context conflict record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_conflict_record_failure(UmiContextConflict *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context conflict satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_conflict_validate(const UmiContextConflict *state);
/**
 * Provide the context conflict covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_conflict_covers_sequence(const UmiContextConflict *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
