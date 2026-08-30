/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/panel_host_model.h
 *
 * PURPOSE:
 *   Define the reusable linked-panel host view model contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_HOST_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_HOST_MODEL_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPanelHostModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char host_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkPanelHostModel;

void umi_workbench_context_link_panel_host_model_init(UmiWorkbenchContextLinkPanelHostModel *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_panel_host_model_validate(
    const UmiWorkbenchContextLinkPanelHostModel *record);
UmiStatus umi_workbench_context_link_panel_host_model_copy(
    UmiWorkbenchContextLinkPanelHostModel *destination,
    const UmiWorkbenchContextLinkPanelHostModel *source);
uint64_t umi_workbench_context_link_panel_host_model_hash(
    const UmiWorkbenchContextLinkPanelHostModel *record);
UmiStatus umi_workbench_context_link_panel_host_model_set_primary(
    UmiWorkbenchContextLinkPanelHostModel *record,
    const char *value);
UmiStatus umi_workbench_context_link_panel_host_model_set_secondary(
    UmiWorkbenchContextLinkPanelHostModel *record,
    const char *value);
void umi_workbench_context_link_panel_host_model_touch(
    UmiWorkbenchContextLinkPanelHostModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
