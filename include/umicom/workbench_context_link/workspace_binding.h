/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/workspace_binding.h
 *
 * PURPOSE:
 *   Define the reusable workspace context binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_BINDING_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link workspace binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkWorkspaceBinding {
    uint32_t structure_size;
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkWorkspaceBinding;

/**
 * Initialise workbench context link workspace binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_workspace_binding_init(UmiWorkbenchContextLinkWorkspaceBinding *record,
                                           const char *identity);
/**
 * Check that workbench context link workspace binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_workspace_binding_validate(
    const UmiWorkbenchContextLinkWorkspaceBinding *record);
/**
 * Copy workbench context link workspace binding into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_workspace_binding_copy(
    UmiWorkbenchContextLinkWorkspaceBinding *destination,
    const UmiWorkbenchContextLinkWorkspaceBinding *source);
/**
 * Provide the workbench context link workspace binding hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_workspace_binding_hash(
    const UmiWorkbenchContextLinkWorkspaceBinding *record);
/**
 * Provide the workbench context link workspace binding set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_workspace_binding_set_primary(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    const char *value);
/**
 * Provide the workbench context link workspace binding set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_workspace_binding_set_secondary(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    const char *value);
/**
 * Provide the workbench context link workspace binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_workspace_binding_touch(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
