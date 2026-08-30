/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/panel_link_state.h
 *
 * PURPOSE:
 *   Define the reusable panel link state record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_LINK_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_LINK_STATE_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPanelLinkState {
    uint32_t structure_size;
    char state_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkPanelLinkState;

void umi_workbench_context_link_panel_link_state_init(UmiWorkbenchContextLinkPanelLinkState *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_panel_link_state_validate(
    const UmiWorkbenchContextLinkPanelLinkState *record);
UmiStatus umi_workbench_context_link_panel_link_state_copy(
    UmiWorkbenchContextLinkPanelLinkState *destination,
    const UmiWorkbenchContextLinkPanelLinkState *source);
uint64_t umi_workbench_context_link_panel_link_state_hash(
    const UmiWorkbenchContextLinkPanelLinkState *record);
UmiStatus umi_workbench_context_link_panel_link_state_set_primary(
    UmiWorkbenchContextLinkPanelLinkState *record,
    const char *value);
UmiStatus umi_workbench_context_link_panel_link_state_set_secondary(
    UmiWorkbenchContextLinkPanelLinkState *record,
    const char *value);
void umi_workbench_context_link_panel_link_state_touch(
    UmiWorkbenchContextLinkPanelLinkState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
