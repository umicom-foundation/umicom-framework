/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/rejection_record.h
 *
 * PURPOSE:
 *   Define the reusable selection rejection record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_REJECTION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_REJECTION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionRejectionRecord {
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
} UmiWorkbenchSelectionRejectionRecord;

void umi_workbench_selection_rejection_record_init(
    UmiWorkbenchSelectionRejectionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_rejection_record_validate(
    const UmiWorkbenchSelectionRejectionRecord *record);
UmiStatus umi_workbench_selection_rejection_record_set_source(
    UmiWorkbenchSelectionRejectionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_rejection_record_set_subject(
    UmiWorkbenchSelectionRejectionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_rejection_record_set_secondary(
    UmiWorkbenchSelectionRejectionRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_rejection_record_set_group(
    UmiWorkbenchSelectionRejectionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_rejection_record_set_label(
    UmiWorkbenchSelectionRejectionRecord *record,
    const char *label);
uint64_t umi_workbench_selection_rejection_record_hash(
    const UmiWorkbenchSelectionRejectionRecord *record);
void umi_workbench_selection_rejection_record_touch(
    UmiWorkbenchSelectionRejectionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
