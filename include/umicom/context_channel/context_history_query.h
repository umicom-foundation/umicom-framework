/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_history_query.h
 *
 * PURPOSE:
 *   Describe bounded history queries over context delivery evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_HISTORY_QUERY_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_HISTORY_QUERY_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextHistoryQuery {
    uint32_t structure_size;
    char query_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char application_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextHistoryQuery;
void umi_context_history_query_init(UmiContextHistoryQuery *state);
UmiStatus umi_context_history_query_set_field(UmiContextHistoryQuery *state,size_t field_index,const char *value);
const char *umi_context_history_query_field(const UmiContextHistoryQuery *state,size_t field_index);
UmiStatus umi_context_history_query_record_success(UmiContextHistoryQuery *state,uint64_t sequence);
UmiStatus umi_context_history_query_record_failure(UmiContextHistoryQuery *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_history_query_validate(const UmiContextHistoryQuery *state);
bool umi_context_history_query_covers_sequence(const UmiContextHistoryQuery *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
