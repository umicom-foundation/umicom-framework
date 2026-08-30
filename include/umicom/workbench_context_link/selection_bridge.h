/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/selection_bridge.h
 *
 * PURPOSE:
 *   Define the reusable selection context bridge contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_SELECTION_BRIDGE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_SELECTION_BRIDGE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkSelectionBridge {
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
} UmiWorkbenchContextLinkSelectionBridge;

void umi_workbench_context_link_selection_bridge_init(UmiWorkbenchContextLinkSelectionBridge *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_selection_bridge_validate(
    const UmiWorkbenchContextLinkSelectionBridge *record);
UmiStatus umi_workbench_context_link_selection_bridge_copy(
    UmiWorkbenchContextLinkSelectionBridge *destination,
    const UmiWorkbenchContextLinkSelectionBridge *source);
uint64_t umi_workbench_context_link_selection_bridge_hash(
    const UmiWorkbenchContextLinkSelectionBridge *record);
UmiStatus umi_workbench_context_link_selection_bridge_set_primary(
    UmiWorkbenchContextLinkSelectionBridge *record,
    const char *value);
UmiStatus umi_workbench_context_link_selection_bridge_set_secondary(
    UmiWorkbenchContextLinkSelectionBridge *record,
    const char *value);
void umi_workbench_context_link_selection_bridge_touch(
    UmiWorkbenchContextLinkSelectionBridge *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
