/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/editor_selection.h
 *
 * PURPOSE:
 *   Define the reusable editor selection snapshot contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_EDITOR_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_EDITOR_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceEditorSelection {
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
} UmiWorkbenchContextSourceEditorSelection;

void umi_workbench_context_source_editor_selection_init(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_editor_selection_validate(
    const UmiWorkbenchContextSourceEditorSelection *record);
UmiStatus umi_workbench_context_source_editor_selection_set_source(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_editor_selection_set_panel(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_editor_selection_set_subject(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_editor_selection_set_group(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_editor_selection_set_label(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *label);
uint64_t umi_workbench_context_source_editor_selection_hash(
    const UmiWorkbenchContextSourceEditorSelection *record);
void umi_workbench_context_source_editor_selection_touch(
    UmiWorkbenchContextSourceEditorSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
