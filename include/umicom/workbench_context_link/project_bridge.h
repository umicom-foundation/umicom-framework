/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/project_bridge.h
 *
 * PURPOSE:
 *   Define the reusable project context bridge contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PROJECT_BRIDGE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PROJECT_BRIDGE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkProjectBridge {
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
} UmiWorkbenchContextLinkProjectBridge;

void umi_workbench_context_link_project_bridge_init(UmiWorkbenchContextLinkProjectBridge *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_project_bridge_validate(
    const UmiWorkbenchContextLinkProjectBridge *record);
UmiStatus umi_workbench_context_link_project_bridge_copy(
    UmiWorkbenchContextLinkProjectBridge *destination,
    const UmiWorkbenchContextLinkProjectBridge *source);
uint64_t umi_workbench_context_link_project_bridge_hash(
    const UmiWorkbenchContextLinkProjectBridge *record);
UmiStatus umi_workbench_context_link_project_bridge_set_primary(
    UmiWorkbenchContextLinkProjectBridge *record,
    const char *value);
UmiStatus umi_workbench_context_link_project_bridge_set_secondary(
    UmiWorkbenchContextLinkProjectBridge *record,
    const char *value);
void umi_workbench_context_link_project_bridge_touch(
    UmiWorkbenchContextLinkProjectBridge *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
