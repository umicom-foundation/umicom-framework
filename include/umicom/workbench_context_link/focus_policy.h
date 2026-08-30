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

void umi_workbench_context_link_focus_policy_init(UmiWorkbenchContextLinkFocusPolicy *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_focus_policy_validate(
    const UmiWorkbenchContextLinkFocusPolicy *record);
UmiStatus umi_workbench_context_link_focus_policy_copy(
    UmiWorkbenchContextLinkFocusPolicy *destination,
    const UmiWorkbenchContextLinkFocusPolicy *source);
uint64_t umi_workbench_context_link_focus_policy_hash(
    const UmiWorkbenchContextLinkFocusPolicy *record);
UmiStatus umi_workbench_context_link_focus_policy_set_primary(
    UmiWorkbenchContextLinkFocusPolicy *record,
    const char *value);
UmiStatus umi_workbench_context_link_focus_policy_set_secondary(
    UmiWorkbenchContextLinkFocusPolicy *record,
    const char *value);
void umi_workbench_context_link_focus_policy_touch(
    UmiWorkbenchContextLinkFocusPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
