/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/route_state.h
 *
 * PURPOSE:
 *   Define the reusable route runtime state record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ROUTE_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ROUTE_STATE_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link route state data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkRouteState {
    uint32_t structure_size;
    char state_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char route_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkRouteState;

/**
 * Initialise workbench context link route state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_route_state_init(UmiWorkbenchContextLinkRouteState *record,
                                           const char *identity);
/**
 * Check that workbench context link route state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_route_state_validate(
    const UmiWorkbenchContextLinkRouteState *record);
/**
 * Copy workbench context link route state into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_route_state_copy(
    UmiWorkbenchContextLinkRouteState *destination,
    const UmiWorkbenchContextLinkRouteState *source);
/**
 * Provide the workbench context link route state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_route_state_hash(
    const UmiWorkbenchContextLinkRouteState *record);
/**
 * Provide the workbench context link route state set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_route_state_set_primary(
    UmiWorkbenchContextLinkRouteState *record,
    const char *value);
/**
 * Provide the workbench context link route state set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_route_state_set_secondary(
    UmiWorkbenchContextLinkRouteState *record,
    const char *value);
/**
 * Provide the workbench context link route state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_route_state_touch(
    UmiWorkbenchContextLinkRouteState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_route_state_archive_encode(const UmiWorkbenchContextLinkRouteState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_route_state_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkRouteState *value);

#ifdef __cplusplus
}
#endif

#endif
