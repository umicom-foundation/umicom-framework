/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/project_selection.h
 *
 * PURPOSE:
 *   Define the reusable project selection snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PROJECT_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PROJECT_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceProjectSelection {
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
} UmiWorkbenchContextSourceProjectSelection;

void umi_workbench_context_source_project_selection_init(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_project_selection_validate(
    const UmiWorkbenchContextSourceProjectSelection *record);
UmiStatus umi_workbench_context_source_project_selection_set_source(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_project_selection_set_panel(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_project_selection_set_subject(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_project_selection_set_group(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_project_selection_set_label(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *label);
uint64_t umi_workbench_context_source_project_selection_hash(
    const UmiWorkbenchContextSourceProjectSelection *record);
void umi_workbench_context_source_project_selection_touch(
    UmiWorkbenchContextSourceProjectSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
