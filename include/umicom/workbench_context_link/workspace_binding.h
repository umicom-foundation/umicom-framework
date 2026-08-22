/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/workspace_binding.h
 *
 * PURPOSE:
 *   Define the reusable workspace context binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_BINDING_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_workspace_binding_init(UmiWorkbenchContextLinkWorkspaceBinding *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_workspace_binding_validate(
    const UmiWorkbenchContextLinkWorkspaceBinding *record);
UmiStatus umi_workbench_context_link_workspace_binding_copy(
    UmiWorkbenchContextLinkWorkspaceBinding *destination,
    const UmiWorkbenchContextLinkWorkspaceBinding *source);
uint64_t umi_workbench_context_link_workspace_binding_hash(
    const UmiWorkbenchContextLinkWorkspaceBinding *record);
UmiStatus umi_workbench_context_link_workspace_binding_set_primary(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    const char *value);
UmiStatus umi_workbench_context_link_workspace_binding_set_secondary(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    const char *value);
void umi_workbench_context_link_workspace_binding_touch(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
