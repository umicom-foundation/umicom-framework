/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/preview_record.h
 *
 * PURPOSE:
 *   Define the reusable selection preview record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PREVIEW_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PREVIEW_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionPreviewRecord {
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
} UmiWorkbenchSelectionPreviewRecord;

void umi_workbench_selection_preview_record_init(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_preview_record_validate(
    const UmiWorkbenchSelectionPreviewRecord *record);
UmiStatus umi_workbench_selection_preview_record_set_source(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_preview_record_set_subject(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_preview_record_set_related(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_preview_record_set_group(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_preview_record_set_description(
    UmiWorkbenchSelectionPreviewRecord *record,
    const char *description);
uint64_t umi_workbench_selection_preview_record_hash(
    const UmiWorkbenchSelectionPreviewRecord *record);
void umi_workbench_selection_preview_record_touch(
    UmiWorkbenchSelectionPreviewRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
