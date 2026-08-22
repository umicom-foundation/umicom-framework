/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/panel_header_model.h
 *
 * PURPOSE:
 *   Define the reusable linked-panel header view model contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_HEADER_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_HEADER_MODEL_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPanelHeaderModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkPanelHeaderModel;

void umi_workbench_context_link_panel_header_model_init(UmiWorkbenchContextLinkPanelHeaderModel *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_panel_header_model_validate(
    const UmiWorkbenchContextLinkPanelHeaderModel *record);
UmiStatus umi_workbench_context_link_panel_header_model_copy(
    UmiWorkbenchContextLinkPanelHeaderModel *destination,
    const UmiWorkbenchContextLinkPanelHeaderModel *source);
uint64_t umi_workbench_context_link_panel_header_model_hash(
    const UmiWorkbenchContextLinkPanelHeaderModel *record);
UmiStatus umi_workbench_context_link_panel_header_model_set_primary(
    UmiWorkbenchContextLinkPanelHeaderModel *record,
    const char *value);
UmiStatus umi_workbench_context_link_panel_header_model_set_secondary(
    UmiWorkbenchContextLinkPanelHeaderModel *record,
    const char *value);
void umi_workbench_context_link_panel_header_model_touch(
    UmiWorkbenchContextLinkPanelHeaderModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
