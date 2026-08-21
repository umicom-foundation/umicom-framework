/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_filter.h
 *
 * PURPOSE:
 *   Record deterministic filter evaluation evidence for routing decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_FILTER_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_FILTER_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_context_filter_init(UmiContextFilter *state);
UmiStatus umi_context_filter_set_field(UmiContextFilter *state,size_t field_index,const char *value);
const char *umi_context_filter_field(const UmiContextFilter *state,size_t field_index);
UmiStatus umi_context_filter_record_success(UmiContextFilter *state,uint64_t sequence);
UmiStatus umi_context_filter_record_failure(UmiContextFilter *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_filter_validate(const UmiContextFilter *state);
bool umi_context_filter_covers_sequence(const UmiContextFilter *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
