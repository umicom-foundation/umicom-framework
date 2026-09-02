/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_replay.h
 *
 * PURPOSE:
 *   Replay retained contexts with a deterministic cursor and delivery budget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_REPLAY_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_REPLAY_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context replay data shared with callers of this public contract.
 */
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
/**
 * Initialise context replay from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_replay_init(UmiContextReplay *state);
/**
 * Provide the context replay set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_replay_set_field(UmiContextReplay *state,size_t field_index,const char *value);
/**
 * Provide the context replay field operation used by this module and its client
 * applications.
 */
const char *umi_context_replay_field(const UmiContextReplay *state,size_t field_index);
/**
 * Provide the context replay record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_replay_record_success(UmiContextReplay *state,uint64_t sequence);
/**
 * Provide the context replay record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_replay_record_failure(UmiContextReplay *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context replay satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_replay_validate(const UmiContextReplay *state);
/**
 * Provide the context replay covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_replay_covers_sequence(const UmiContextReplay *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
