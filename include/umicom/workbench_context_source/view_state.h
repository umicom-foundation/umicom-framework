/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/view_state.h
 *
 * PURPOSE:
 *   Define the reusable view state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_VIEW_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_VIEW_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceViewState {
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
} UmiWorkbenchContextSourceViewState;

void umi_workbench_context_source_view_state_init(
    UmiWorkbenchContextSourceViewState *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_view_state_validate(
    const UmiWorkbenchContextSourceViewState *record);
UmiStatus umi_workbench_context_source_view_state_set_source(
    UmiWorkbenchContextSourceViewState *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_view_state_set_panel(
    UmiWorkbenchContextSourceViewState *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_view_state_set_subject(
    UmiWorkbenchContextSourceViewState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_view_state_set_group(
    UmiWorkbenchContextSourceViewState *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_view_state_set_label(
    UmiWorkbenchContextSourceViewState *record,
    const char *label);
uint64_t umi_workbench_context_source_view_state_hash(
    const UmiWorkbenchContextSourceViewState *record);
void umi_workbench_context_source_view_state_touch(
    UmiWorkbenchContextSourceViewState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
