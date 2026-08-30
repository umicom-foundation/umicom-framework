/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/panel_binding.h
 *
 * PURPOSE:
 *   Define the reusable panel-to-colour-group binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_BINDING_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPanelBinding {
    uint32_t structure_size;
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkPanelBinding;

void umi_workbench_context_link_panel_binding_init(UmiWorkbenchContextLinkPanelBinding *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_panel_binding_validate(
    const UmiWorkbenchContextLinkPanelBinding *record);
UmiStatus umi_workbench_context_link_panel_binding_copy(
    UmiWorkbenchContextLinkPanelBinding *destination,
    const UmiWorkbenchContextLinkPanelBinding *source);
uint64_t umi_workbench_context_link_panel_binding_hash(
    const UmiWorkbenchContextLinkPanelBinding *record);
UmiStatus umi_workbench_context_link_panel_binding_set_primary(
    UmiWorkbenchContextLinkPanelBinding *record,
    const char *value);
UmiStatus umi_workbench_context_link_panel_binding_set_secondary(
    UmiWorkbenchContextLinkPanelBinding *record,
    const char *value);
void umi_workbench_context_link_panel_binding_touch(
    UmiWorkbenchContextLinkPanelBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
