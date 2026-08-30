/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/duplicate_record.h
 *
 * PURPOSE:
 *   Define the reusable duplicate selection record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_DUPLICATE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_DUPLICATE_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionDuplicateRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionDuplicateRecord;

void umi_workbench_selection_duplicate_record_init(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_duplicate_record_validate(
    const UmiWorkbenchSelectionDuplicateRecord *record);
UmiStatus umi_workbench_selection_duplicate_record_set_source(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_duplicate_record_set_subject(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_duplicate_record_set_secondary(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_duplicate_record_set_group(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_duplicate_record_set_label(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *label);
uint64_t umi_workbench_selection_duplicate_record_hash(
    const UmiWorkbenchSelectionDuplicateRecord *record);
void umi_workbench_selection_duplicate_record_touch(
    UmiWorkbenchSelectionDuplicateRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
