/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_export.h
 *
 * PURPOSE:
 *   Plan portable context exports without leaking application-private state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_EXPORT_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_EXPORT_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextExport {
    uint32_t structure_size;
    char export_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char destination[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextExport;
void umi_context_export_init(UmiContextExport *state);
UmiStatus umi_context_export_set_field(UmiContextExport *state,size_t field_index,const char *value);
const char *umi_context_export_field(const UmiContextExport *state,size_t field_index);
UmiStatus umi_context_export_record_success(UmiContextExport *state,uint64_t sequence);
UmiStatus umi_context_export_record_failure(UmiContextExport *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_export_validate(const UmiContextExport *state);
bool umi_context_export_covers_sequence(const UmiContextExport *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
