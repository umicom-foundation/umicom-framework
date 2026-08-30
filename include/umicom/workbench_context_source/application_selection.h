/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/application_selection.h
 *
 * PURPOSE:
 *   Define the reusable application selection snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_APPLICATION_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_APPLICATION_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceApplicationSelection {
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
} UmiWorkbenchContextSourceApplicationSelection;

void umi_workbench_context_source_application_selection_init(
    UmiWorkbenchContextSourceApplicationSelection *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_application_selection_validate(
    const UmiWorkbenchContextSourceApplicationSelection *record);
UmiStatus umi_workbench_context_source_application_selection_set_source(
    UmiWorkbenchContextSourceApplicationSelection *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_application_selection_set_panel(
    UmiWorkbenchContextSourceApplicationSelection *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_application_selection_set_subject(
    UmiWorkbenchContextSourceApplicationSelection *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_application_selection_set_group(
    UmiWorkbenchContextSourceApplicationSelection *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_application_selection_set_label(
    UmiWorkbenchContextSourceApplicationSelection *record,
    const char *label);
uint64_t umi_workbench_context_source_application_selection_hash(
    const UmiWorkbenchContextSourceApplicationSelection *record);
void umi_workbench_context_source_application_selection_touch(
    UmiWorkbenchContextSourceApplicationSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
