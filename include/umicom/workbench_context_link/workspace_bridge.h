/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/workspace_bridge.h
 *
 * PURPOSE:
 *   Define the reusable workspace context bridge contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_BRIDGE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_BRIDGE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkWorkspaceBridge {
    uint32_t structure_size;
    char bridge_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkWorkspaceBridge;

void umi_workbench_context_link_workspace_bridge_init(UmiWorkbenchContextLinkWorkspaceBridge *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_workspace_bridge_validate(
    const UmiWorkbenchContextLinkWorkspaceBridge *record);
UmiStatus umi_workbench_context_link_workspace_bridge_copy(
    UmiWorkbenchContextLinkWorkspaceBridge *destination,
    const UmiWorkbenchContextLinkWorkspaceBridge *source);
uint64_t umi_workbench_context_link_workspace_bridge_hash(
    const UmiWorkbenchContextLinkWorkspaceBridge *record);
UmiStatus umi_workbench_context_link_workspace_bridge_set_primary(
    UmiWorkbenchContextLinkWorkspaceBridge *record,
    const char *value);
UmiStatus umi_workbench_context_link_workspace_bridge_set_secondary(
    UmiWorkbenchContextLinkWorkspaceBridge *record,
    const char *value);
void umi_workbench_context_link_workspace_bridge_touch(
    UmiWorkbenchContextLinkWorkspaceBridge *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
