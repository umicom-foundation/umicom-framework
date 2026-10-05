/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/sync_state.h
 *
 * PURPOSE:
 *   Define the reusable context-link synchronisation state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_SYNC_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_SYNC_STATE_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link sync state data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkSyncState {
    uint32_t structure_size;
    char sync_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char peer_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkSyncState;

/**
 * Initialise workbench context link sync state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_sync_state_init(UmiWorkbenchContextLinkSyncState *record,
                                           const char *identity);
/**
 * Check that workbench context link sync state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_sync_state_validate(
    const UmiWorkbenchContextLinkSyncState *record);
/**
 * Copy workbench context link sync state into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_sync_state_copy(
    UmiWorkbenchContextLinkSyncState *destination,
    const UmiWorkbenchContextLinkSyncState *source);
/**
 * Provide the workbench context link sync state hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_sync_state_hash(
    const UmiWorkbenchContextLinkSyncState *record);
/**
 * Provide the workbench context link sync state set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_sync_state_set_primary(
    UmiWorkbenchContextLinkSyncState *record,
    const char *value);
/**
 * Provide the workbench context link sync state set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_sync_state_set_secondary(
    UmiWorkbenchContextLinkSyncState *record,
    const char *value);
/**
 * Provide the workbench context link sync state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_sync_state_touch(
    UmiWorkbenchContextLinkSyncState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_sync_state_archive_encode(const UmiWorkbenchContextLinkSyncState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_sync_state_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkSyncState *value);

#ifdef __cplusplus
}
#endif

#endif
