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

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_sync_state_init(UmiWorkbenchContextLinkSyncState *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_sync_state_validate(
    const UmiWorkbenchContextLinkSyncState *record);
UmiStatus umi_workbench_context_link_sync_state_copy(
    UmiWorkbenchContextLinkSyncState *destination,
    const UmiWorkbenchContextLinkSyncState *source);
uint64_t umi_workbench_context_link_sync_state_hash(
    const UmiWorkbenchContextLinkSyncState *record);
UmiStatus umi_workbench_context_link_sync_state_set_primary(
    UmiWorkbenchContextLinkSyncState *record,
    const char *value);
UmiStatus umi_workbench_context_link_sync_state_set_secondary(
    UmiWorkbenchContextLinkSyncState *record,
    const char *value);
void umi_workbench_context_link_sync_state_touch(
    UmiWorkbenchContextLinkSyncState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
