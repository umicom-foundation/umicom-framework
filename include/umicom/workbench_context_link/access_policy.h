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

/**
 * Represent the workbench context link access policy data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context link access policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_access_policy_init(UmiWorkbenchContextLinkAccessPolicy *record,
                                           const char *identity);
/**
 * Check that workbench context link access policy satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_access_policy_validate(
    const UmiWorkbenchContextLinkAccessPolicy *record);
/**
 * Copy workbench context link access policy into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_access_policy_copy(
    UmiWorkbenchContextLinkAccessPolicy *destination,
    const UmiWorkbenchContextLinkAccessPolicy *source);
/**
 * Provide the workbench context link access policy hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_access_policy_hash(
    const UmiWorkbenchContextLinkAccessPolicy *record);
/**
 * Provide the workbench context link access policy set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_access_policy_set_primary(
    UmiWorkbenchContextLinkAccessPolicy *record,
    const char *value);
/**
 * Provide the workbench context link access policy set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_access_policy_set_secondary(
    UmiWorkbenchContextLinkAccessPolicy *record,
    const char *value);
/**
 * Provide the workbench context link access policy touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_access_policy_touch(
    UmiWorkbenchContextLinkAccessPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
