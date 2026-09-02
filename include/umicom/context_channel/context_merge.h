/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_merge.h
 *
 * PURPOSE:
 *   Record explicit context merge choices and resulting evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_MERGE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_MERGE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context merge data shared with callers of this public contract.
 */
typedef struct UmiContextMerge {
    uint32_t structure_size;
    char merge_id[UMI_CONTEXT_VALUE_CAPACITY];
    char conflict_id[UMI_CONTEXT_VALUE_CAPACITY];
    char resolution[UMI_CONTEXT_VALUE_CAPACITY];
    char result_context_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextMerge;
/**
 * Initialise context merge from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_merge_init(UmiContextMerge *state);
/**
 * Provide the context merge set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_merge_set_field(UmiContextMerge *state,size_t field_index,const char *value);
/**
 * Provide the context merge field operation used by this module and its client
 * applications.
 */
const char *umi_context_merge_field(const UmiContextMerge *state,size_t field_index);
/**
 * Provide the context merge record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_merge_record_success(UmiContextMerge *state,uint64_t sequence);
/**
 * Provide the context merge record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_merge_record_failure(UmiContextMerge *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context merge satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_merge_validate(const UmiContextMerge *state);
/**
 * Provide the context merge covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_merge_covers_sequence(const UmiContextMerge *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
