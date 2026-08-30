/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/access_policy.h
 *
 * PURPOSE:
 *   Define the reusable context-link access policy contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACCESS_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACCESS_POLICY_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkAccessPolicy {
    uint32_t structure_size;
    char policy_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char resource_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkAccessPolicy;

void umi_workbench_context_link_access_policy_init(UmiWorkbenchContextLinkAccessPolicy *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_access_policy_validate(
    const UmiWorkbenchContextLinkAccessPolicy *record);
UmiStatus umi_workbench_context_link_access_policy_copy(
    UmiWorkbenchContextLinkAccessPolicy *destination,
    const UmiWorkbenchContextLinkAccessPolicy *source);
uint64_t umi_workbench_context_link_access_policy_hash(
    const UmiWorkbenchContextLinkAccessPolicy *record);
UmiStatus umi_workbench_context_link_access_policy_set_primary(
    UmiWorkbenchContextLinkAccessPolicy *record,
    const char *value);
UmiStatus umi_workbench_context_link_access_policy_set_secondary(
    UmiWorkbenchContextLinkAccessPolicy *record,
    const char *value);
void umi_workbench_context_link_access_policy_touch(
    UmiWorkbenchContextLinkAccessPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
