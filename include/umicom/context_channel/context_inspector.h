/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_inspector.h
 *
 * PURPOSE:
 *   Expose a read-only property projection for one typed context payload.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_INSPECTOR_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_INSPECTOR_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextInspector {
    uint32_t structure_size;
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char source_application[UMI_CONTEXT_VALUE_CAPACITY];
    char source_panel[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextInspector;
void umi_context_inspector_init(UmiContextInspector *state);
UmiStatus umi_context_inspector_set_field(UmiContextInspector *state,size_t field_index,const char *value);
const char *umi_context_inspector_field(const UmiContextInspector *state,size_t field_index);
UmiStatus umi_context_inspector_record_success(UmiContextInspector *state,uint64_t sequence);
UmiStatus umi_context_inspector_record_failure(UmiContextInspector *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_inspector_validate(const UmiContextInspector *state);
bool umi_context_inspector_covers_sequence(const UmiContextInspector *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
