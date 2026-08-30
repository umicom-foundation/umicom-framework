/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/activation_policy.h
 *
 * PURPOSE:
 *   Define the reusable panel activation policy contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACTIVATION_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACTIVATION_POLICY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkActivationPolicy {
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
} UmiWorkbenchContextLinkActivationPolicy;

void umi_workbench_context_link_activation_policy_init(UmiWorkbenchContextLinkActivationPolicy *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_activation_policy_validate(
    const UmiWorkbenchContextLinkActivationPolicy *record);
UmiStatus umi_workbench_context_link_activation_policy_copy(
    UmiWorkbenchContextLinkActivationPolicy *destination,
    const UmiWorkbenchContextLinkActivationPolicy *source);
uint64_t umi_workbench_context_link_activation_policy_hash(
    const UmiWorkbenchContextLinkActivationPolicy *record);
UmiStatus umi_workbench_context_link_activation_policy_set_primary(
    UmiWorkbenchContextLinkActivationPolicy *record,
    const char *value);
UmiStatus umi_workbench_context_link_activation_policy_set_secondary(
    UmiWorkbenchContextLinkActivationPolicy *record,
    const char *value);
void umi_workbench_context_link_activation_policy_touch(
    UmiWorkbenchContextLinkActivationPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
