/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_snapshot_store.h
 *
 * PURPOSE:
 *   Retain named immutable context snapshots for debugging and comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_SNAPSHOT_STORE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_SNAPSHOT_STORE_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context snapshot store data shared with callers of this public contract.
 */
typedef struct UmiContextSnapshotStore {
    uint32_t structure_size;
    char snapshot_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char label[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextSnapshotStore;
/**
 * Initialise context snapshot store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_snapshot_store_init(UmiContextSnapshotStore *state);
/**
 * Provide the context snapshot store set field operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_snapshot_store_set_field(UmiContextSnapshotStore *state,size_t field_index,const char *value);
/**
 * Provide the context snapshot store field operation used by this module and its client
 * applications.
 */
const char *umi_context_snapshot_store_field(const UmiContextSnapshotStore *state,size_t field_index);
/**
 * Provide the context snapshot store record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_snapshot_store_record_success(UmiContextSnapshotStore *state,uint64_t sequence);
/**
 * Provide the context snapshot store record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_snapshot_store_record_failure(UmiContextSnapshotStore *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context snapshot store satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_snapshot_store_validate(const UmiContextSnapshotStore *state);
/**
 * Provide the context snapshot store covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_snapshot_store_covers_sequence(const UmiContextSnapshotStore *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
