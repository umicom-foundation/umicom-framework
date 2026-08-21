/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_validator.h
 *
 * PURPOSE:
 *   Aggregate structural, schema, policy and expiry validation evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_VALIDATOR_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_VALIDATOR_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextValidator {
    uint32_t structure_size;
    char validation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextValidator;
void umi_context_validator_init(UmiContextValidator *state);
UmiStatus umi_context_validator_set_field(UmiContextValidator *state,size_t field_index,const char *value);
const char *umi_context_validator_field(const UmiContextValidator *state,size_t field_index);
UmiStatus umi_context_validator_record_success(UmiContextValidator *state,uint64_t sequence);
UmiStatus umi_context_validator_record_failure(UmiContextValidator *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_validator_validate(const UmiContextValidator *state);
bool umi_context_validator_covers_sequence(const UmiContextValidator *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
