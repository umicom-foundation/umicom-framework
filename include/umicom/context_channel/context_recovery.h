/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_recovery.h
 *
 * PURPOSE:
 *   Retain context recovery checkpoints after application or process failure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_RECOVERY_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_RECOVERY_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextRecovery {
    uint32_t structure_size;
    char recovery_id[UMI_CONTEXT_VALUE_CAPACITY];
    char session_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char checkpoint_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextRecovery;
void umi_context_recovery_init(UmiContextRecovery *state);
UmiStatus umi_context_recovery_set_field(UmiContextRecovery *state,size_t field_index,const char *value);
const char *umi_context_recovery_field(const UmiContextRecovery *state,size_t field_index);
UmiStatus umi_context_recovery_record_success(UmiContextRecovery *state,uint64_t sequence);
UmiStatus umi_context_recovery_record_failure(UmiContextRecovery *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_recovery_validate(const UmiContextRecovery *state);
bool umi_context_recovery_covers_sequence(const UmiContextRecovery *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
