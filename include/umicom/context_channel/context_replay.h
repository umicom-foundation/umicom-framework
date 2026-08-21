/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_replay.h
 *
 * PURPOSE:
 *   Replay retained contexts with a deterministic cursor and delivery budget.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_REPLAY_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_REPLAY_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiContextReplay {
    uint32_t structure_size;
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char from_context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char target_application[UMI_CONTEXT_VALUE_CAPACITY];
    char target_panel[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextReplay;
void umi_context_replay_init(UmiContextReplay *state);
UmiStatus umi_context_replay_set_field(UmiContextReplay *state,size_t field_index,const char *value);
const char *umi_context_replay_field(const UmiContextReplay *state,size_t field_index);
UmiStatus umi_context_replay_record_success(UmiContextReplay *state,uint64_t sequence);
UmiStatus umi_context_replay_record_failure(UmiContextReplay *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_replay_validate(const UmiContextReplay *state);
bool umi_context_replay_covers_sequence(const UmiContextReplay *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
