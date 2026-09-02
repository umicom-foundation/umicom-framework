/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_search.h
 *
 * PURPOSE:
 *   Search context history, panels and channels using bounded query terms.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_SEARCH_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_SEARCH_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context search data shared with callers of this public contract.
 */
typedef struct UmiContextSearch {
    uint32_t structure_size;
    char query[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char application_id[UMI_CONTEXT_VALUE_CAPACITY];
    char panel_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextSearch;
/**
 * Initialise context search from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_search_init(UmiContextSearch *state);
/**
 * Provide the context search set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_search_set_field(UmiContextSearch *state,size_t field_index,const char *value);
/**
 * Provide the context search field operation used by this module and its client
 * applications.
 */
const char *umi_context_search_field(const UmiContextSearch *state,size_t field_index);
/**
 * Provide the context search record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_search_record_success(UmiContextSearch *state,uint64_t sequence);
/**
 * Provide the context search record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_search_record_failure(UmiContextSearch *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context search satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_search_validate(const UmiContextSearch *state);
/**
 * Provide the context search covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_search_covers_sequence(const UmiContextSearch *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
