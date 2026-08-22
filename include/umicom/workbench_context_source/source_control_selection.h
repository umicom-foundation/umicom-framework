/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/source_control_selection.h
 *
 * PURPOSE:
 *   Define the reusable source-control selection snapshot contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceSourceControlSelection {
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
} UmiWorkbenchContextSourceSourceControlSelection;

void umi_workbench_context_source_source_control_selection_init(
    UmiWorkbenchContextSourceSourceControlSelection *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_source_control_selection_validate(
    const UmiWorkbenchContextSourceSourceControlSelection *record);
UmiStatus umi_workbench_context_source_source_control_selection_set_source(
    UmiWorkbenchContextSourceSourceControlSelection *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_source_control_selection_set_panel(
    UmiWorkbenchContextSourceSourceControlSelection *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_source_control_selection_set_subject(
    UmiWorkbenchContextSourceSourceControlSelection *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_source_control_selection_set_group(
    UmiWorkbenchContextSourceSourceControlSelection *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_source_control_selection_set_label(
    UmiWorkbenchContextSourceSourceControlSelection *record,
    const char *label);
uint64_t umi_workbench_context_source_source_control_selection_hash(
    const UmiWorkbenchContextSourceSourceControlSelection *record);
void umi_workbench_context_source_source_control_selection_touch(
    UmiWorkbenchContextSourceSourceControlSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
