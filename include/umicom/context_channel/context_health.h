/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_health.h
 *
 * PURPOSE:
 *   Expose aggregate context-channel health and degradation evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_HEALTH_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_HEALTH_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextHealth {
    uint32_t structure_size;
    char health_id[UMI_CONTEXT_VALUE_CAPACITY];
    char component_id[UMI_CONTEXT_VALUE_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    char last_failure[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextHealth;
void umi_context_health_init(UmiContextHealth *state);
UmiStatus umi_context_health_set_field(UmiContextHealth *state,size_t field_index,const char *value);
const char *umi_context_health_field(const UmiContextHealth *state,size_t field_index);
UmiStatus umi_context_health_record_success(UmiContextHealth *state,uint64_t sequence);
UmiStatus umi_context_health_record_failure(UmiContextHealth *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_health_validate(const UmiContextHealth *state);
bool umi_context_health_covers_sequence(const UmiContextHealth *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
