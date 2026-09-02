/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/focus_policy.h
 *
 * PURPOSE:
 *   Define the reusable focus propagation policy contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_FOCUS_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_FOCUS_POLICY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link focus policy data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkFocusPolicy {
    uint32_t structure_size;
    char policy_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkFocusPolicy;

/**
 * Initialise workbench context link focus policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_focus_policy_init(UmiWorkbenchContextLinkFocusPolicy *record,
                                           const char *identity);
/**
 * Check that workbench context link focus policy satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_focus_policy_validate(
    const UmiWorkbenchContextLinkFocusPolicy *record);
/**
 * Copy workbench context link focus policy into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_focus_policy_copy(
    UmiWorkbenchContextLinkFocusPolicy *destination,
    const UmiWorkbenchContextLinkFocusPolicy *source);
/**
 * Provide the workbench context link focus policy hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_focus_policy_hash(
    const UmiWorkbenchContextLinkFocusPolicy *record);
/**
 * Provide the workbench context link focus policy set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_focus_policy_set_primary(
    UmiWorkbenchContextLinkFocusPolicy *record,
    const char *value);
/**
 * Provide the workbench context link focus policy set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_focus_policy_set_secondary(
    UmiWorkbenchContextLinkFocusPolicy *record,
    const char *value);
/**
 * Provide the workbench context link focus policy touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_focus_policy_touch(
    UmiWorkbenchContextLinkFocusPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
