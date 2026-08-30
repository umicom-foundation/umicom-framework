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
void umi_context_search_init(UmiContextSearch *state);
UmiStatus umi_context_search_set_field(UmiContextSearch *state,size_t field_index,const char *value);
const char *umi_context_search_field(const UmiContextSearch *state,size_t field_index);
UmiStatus umi_context_search_record_success(UmiContextSearch *state,uint64_t sequence);
UmiStatus umi_context_search_record_failure(UmiContextSearch *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_search_validate(const UmiContextSearch *state);
bool umi_context_search_covers_sequence(const UmiContextSearch *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
