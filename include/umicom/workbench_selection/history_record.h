/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/history_record.h
 *
 * PURPOSE:
 *   Define the reusable selection history record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_HISTORY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_HISTORY_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionHistoryRecord {
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
} UmiWorkbenchSelectionHistoryRecord;

void umi_workbench_selection_history_record_init(
    UmiWorkbenchSelectionHistoryRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_history_record_validate(
    const UmiWorkbenchSelectionHistoryRecord *record);
UmiStatus umi_workbench_selection_history_record_set_source(
    UmiWorkbenchSelectionHistoryRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_history_record_set_subject(
    UmiWorkbenchSelectionHistoryRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_history_record_set_secondary(
    UmiWorkbenchSelectionHistoryRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_history_record_set_group(
    UmiWorkbenchSelectionHistoryRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_history_record_set_label(
    UmiWorkbenchSelectionHistoryRecord *record,
    const char *label);
uint64_t umi_workbench_selection_history_record_hash(
    const UmiWorkbenchSelectionHistoryRecord *record);
void umi_workbench_selection_history_record_touch(
    UmiWorkbenchSelectionHistoryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
