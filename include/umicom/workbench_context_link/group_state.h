/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/group_state.h
 *
 * PURPOSE:
 *   Define the reusable group runtime state record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_STATE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link group state data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkGroupState {
    uint32_t structure_size;
    char state_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkGroupState;

/**
 * Initialise workbench context link group state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_group_state_init(UmiWorkbenchContextLinkGroupState *record,
                                           const char *identity);
/**
 * Check that workbench context link group state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_group_state_validate(
    const UmiWorkbenchContextLinkGroupState *record);
/**
 * Copy workbench context link group state into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_group_state_copy(
    UmiWorkbenchContextLinkGroupState *destination,
    const UmiWorkbenchContextLinkGroupState *source);
/**
 * Provide the workbench context link group state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_group_state_hash(
    const UmiWorkbenchContextLinkGroupState *record);
/**
 * Provide the workbench context link group state set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_group_state_set_primary(
    UmiWorkbenchContextLinkGroupState *record,
    const char *value);
/**
 * Provide the workbench context link group state set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_group_state_set_secondary(
    UmiWorkbenchContextLinkGroupState *record,
    const char *value);
/**
 * Provide the workbench context link group state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_group_state_touch(
    UmiWorkbenchContextLinkGroupState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
