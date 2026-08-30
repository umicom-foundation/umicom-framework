/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_transform.h
 *
 * PURPOSE:
 *   Record deterministic schema transformation plans for context values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_TRANSFORM_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_TRANSFORM_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextTransform {
    uint32_t structure_size;
    char plan_id[UMI_CONTEXT_VALUE_CAPACITY];
    char source_schema[UMI_CONTEXT_VALUE_CAPACITY];
    char target_schema[UMI_CONTEXT_VALUE_CAPACITY];
    char transformer_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextTransform;
void umi_context_transform_init(UmiContextTransform *state);
UmiStatus umi_context_transform_set_field(UmiContextTransform *state,size_t field_index,const char *value);
const char *umi_context_transform_field(const UmiContextTransform *state,size_t field_index);
UmiStatus umi_context_transform_record_success(UmiContextTransform *state,uint64_t sequence);
UmiStatus umi_context_transform_record_failure(UmiContextTransform *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_transform_validate(const UmiContextTransform *state);
bool umi_context_transform_covers_sequence(const UmiContextTransform *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
