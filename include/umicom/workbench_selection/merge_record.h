/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/merge_record.h
 *
 * PURPOSE:
 *   Define the reusable selection merge record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_MERGE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_MERGE_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionMergeRecord {
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
} UmiWorkbenchSelectionMergeRecord;

void umi_workbench_selection_merge_record_init(
    UmiWorkbenchSelectionMergeRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_merge_record_validate(
    const UmiWorkbenchSelectionMergeRecord *record);
UmiStatus umi_workbench_selection_merge_record_set_source(
    UmiWorkbenchSelectionMergeRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_merge_record_set_subject(
    UmiWorkbenchSelectionMergeRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_merge_record_set_related(
    UmiWorkbenchSelectionMergeRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_merge_record_set_group(
    UmiWorkbenchSelectionMergeRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_merge_record_set_description(
    UmiWorkbenchSelectionMergeRecord *record,
    const char *description);
uint64_t umi_workbench_selection_merge_record_hash(
    const UmiWorkbenchSelectionMergeRecord *record);
void umi_workbench_selection_merge_record_touch(
    UmiWorkbenchSelectionMergeRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
