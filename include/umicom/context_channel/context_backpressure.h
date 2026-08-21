/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_backpressure.h
 *
 * PURPOSE:
 *   Expose subscriber backlog and delivery pressure for operational control.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_BACKPRESSURE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_BACKPRESSURE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextBackpressure {
    uint32_t structure_size;
    char pressure_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char subscription_id[UMI_CONTEXT_VALUE_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextBackpressure;
void umi_context_backpressure_init(UmiContextBackpressure *state);
UmiStatus umi_context_backpressure_set_field(UmiContextBackpressure *state,size_t field_index,const char *value);
const char *umi_context_backpressure_field(const UmiContextBackpressure *state,size_t field_index);
UmiStatus umi_context_backpressure_record_success(UmiContextBackpressure *state,uint64_t sequence);
UmiStatus umi_context_backpressure_record_failure(UmiContextBackpressure *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_backpressure_validate(const UmiContextBackpressure *state);
bool umi_context_backpressure_covers_sequence(const UmiContextBackpressure *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
