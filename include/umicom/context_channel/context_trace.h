/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_trace.h
 *
 * PURPOSE:
 *   Trace context routing across applications and panels with causation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_TRACE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_TRACE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextTrace {
    uint32_t structure_size;
    char trace_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char correlation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char route_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextTrace;
void umi_context_trace_init(UmiContextTrace *state);
UmiStatus umi_context_trace_set_field(UmiContextTrace *state,size_t field_index,const char *value);
const char *umi_context_trace_field(const UmiContextTrace *state,size_t field_index);
UmiStatus umi_context_trace_record_success(UmiContextTrace *state,uint64_t sequence);
UmiStatus umi_context_trace_record_failure(UmiContextTrace *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_trace_validate(const UmiContextTrace *state);
bool umi_context_trace_covers_sequence(const UmiContextTrace *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
