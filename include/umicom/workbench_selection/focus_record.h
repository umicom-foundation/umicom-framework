/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/focus_record.h
 *
 * PURPOSE:
 *   Define the reusable selection focus record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FOCUS_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_FOCUS_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionFocusRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionFocusRecord;

void umi_workbench_selection_focus_record_init(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_focus_record_validate(
    const UmiWorkbenchSelectionFocusRecord *record);
UmiStatus umi_workbench_selection_focus_record_set_source(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_focus_record_set_subject(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_focus_record_set_related(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_focus_record_set_group(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_focus_record_set_description(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *description);
uint64_t umi_workbench_selection_focus_record_hash(
    const UmiWorkbenchSelectionFocusRecord *record);
void umi_workbench_selection_focus_record_touch(
    UmiWorkbenchSelectionFocusRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
