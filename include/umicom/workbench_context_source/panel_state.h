/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/panel_state.h
 *
 * PURPOSE:
 *   Define the reusable panel state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PANEL_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PANEL_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourcePanelState {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourcePanelState;

void umi_workbench_context_source_panel_state_init(
    UmiWorkbenchContextSourcePanelState *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_panel_state_validate(
    const UmiWorkbenchContextSourcePanelState *record);
UmiStatus umi_workbench_context_source_panel_state_set_source(
    UmiWorkbenchContextSourcePanelState *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_panel_state_set_panel(
    UmiWorkbenchContextSourcePanelState *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_panel_state_set_subject(
    UmiWorkbenchContextSourcePanelState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_panel_state_set_group(
    UmiWorkbenchContextSourcePanelState *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_panel_state_set_label(
    UmiWorkbenchContextSourcePanelState *record,
    const char *label);
uint64_t umi_workbench_context_source_panel_state_hash(
    const UmiWorkbenchContextSourcePanelState *record);
void umi_workbench_context_source_panel_state_touch(
    UmiWorkbenchContextSourcePanelState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
