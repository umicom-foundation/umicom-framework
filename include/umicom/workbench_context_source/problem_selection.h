/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/problem_selection.h
 *
 * PURPOSE:
 *   Define the reusable problem selection snapshot contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PROBLEM_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PROBLEM_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceProblemSelection {
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
} UmiWorkbenchContextSourceProblemSelection;

void umi_workbench_context_source_problem_selection_init(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_problem_selection_validate(
    const UmiWorkbenchContextSourceProblemSelection *record);
UmiStatus umi_workbench_context_source_problem_selection_set_source(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_problem_selection_set_panel(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_problem_selection_set_subject(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_problem_selection_set_group(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_problem_selection_set_label(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *label);
uint64_t umi_workbench_context_source_problem_selection_hash(
    const UmiWorkbenchContextSourceProblemSelection *record);
void umi_workbench_context_source_problem_selection_touch(
    UmiWorkbenchContextSourceProblemSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
