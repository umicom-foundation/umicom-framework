/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_schema_compatibility.h
 *
 * PURPOSE:
 *   Evaluate schema compatibility before cross-application delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_SCHEMA_COMPATIBILITY_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_SCHEMA_COMPATIBILITY_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextSchemaCompatibility {
    uint32_t structure_size;
    char check_id[UMI_CONTEXT_VALUE_CAPACITY];
    char source_schema[UMI_CONTEXT_VALUE_CAPACITY];
    char target_schema[UMI_CONTEXT_VALUE_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextSchemaCompatibility;
void umi_context_schema_compatibility_init(UmiContextSchemaCompatibility *state);
UmiStatus umi_context_schema_compatibility_set_field(UmiContextSchemaCompatibility *state,size_t field_index,const char *value);
const char *umi_context_schema_compatibility_field(const UmiContextSchemaCompatibility *state,size_t field_index);
UmiStatus umi_context_schema_compatibility_record_success(UmiContextSchemaCompatibility *state,uint64_t sequence);
UmiStatus umi_context_schema_compatibility_record_failure(UmiContextSchemaCompatibility *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_schema_compatibility_validate(const UmiContextSchemaCompatibility *state);
bool umi_context_schema_compatibility_covers_sequence(const UmiContextSchemaCompatibility *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
