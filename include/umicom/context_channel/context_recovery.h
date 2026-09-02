/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_recovery.h
 *
 * PURPOSE:
 *   Retain context recovery checkpoints after application or process failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_RECOVERY_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_RECOVERY_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context recovery data shared with callers of this public contract.
 */
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
/**
 * Initialise context recovery from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_recovery_init(UmiContextRecovery *state);
/**
 * Provide the context recovery set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_recovery_set_field(UmiContextRecovery *state,size_t field_index,const char *value);
/**
 * Provide the context recovery field operation used by this module and its client
 * applications.
 */
const char *umi_context_recovery_field(const UmiContextRecovery *state,size_t field_index);
/**
 * Provide the context recovery record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_recovery_record_success(UmiContextRecovery *state,uint64_t sequence);
/**
 * Provide the context recovery record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_recovery_record_failure(UmiContextRecovery *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context recovery satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_recovery_validate(const UmiContextRecovery *state);
/**
 * Provide the context recovery covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_recovery_covers_sequence(const UmiContextRecovery *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
