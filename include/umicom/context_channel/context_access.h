/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_access.h
 *
 * PURPOSE:
 *   Evaluate user and workload access to channel operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_ACCESS_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_ACCESS_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context access data shared with callers of this public contract.
 */
typedef struct UmiContextAccess {
    uint32_t structure_size;
    char access_id[UMI_CONTEXT_VALUE_CAPACITY];
    char subject_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char action_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextAccess;
/**
 * Initialise context access from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_access_init(UmiContextAccess *state);
/**
 * Provide the context access set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_access_set_field(UmiContextAccess *state,size_t field_index,const char *value);
/**
 * Provide the context access field operation used by this module and its client
 * applications.
 */
const char *umi_context_access_field(const UmiContextAccess *state,size_t field_index);
/**
 * Provide the context access record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_access_record_success(UmiContextAccess *state,uint64_t sequence);
/**
 * Provide the context access record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_access_record_failure(UmiContextAccess *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context access satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_access_validate(const UmiContextAccess *state);
/**
 * Provide the context access covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_access_covers_sequence(const UmiContextAccess *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
