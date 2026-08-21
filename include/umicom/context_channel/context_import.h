/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_import.h
 *
 * PURPOSE:
 *   Validate and stage imported context records before publication.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_IMPORT_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_IMPORT_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextImport {
    uint32_t structure_size;
    char import_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char source_name[UMI_CONTEXT_VALUE_CAPACITY];
    char target_channel[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextImport;
void umi_context_import_init(UmiContextImport *state);
UmiStatus umi_context_import_set_field(UmiContextImport *state,size_t field_index,const char *value);
const char *umi_context_import_field(const UmiContextImport *state,size_t field_index);
UmiStatus umi_context_import_record_success(UmiContextImport *state,uint64_t sequence);
UmiStatus umi_context_import_record_failure(UmiContextImport *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_import_validate(const UmiContextImport *state);
bool umi_context_import_covers_sequence(const UmiContextImport *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
