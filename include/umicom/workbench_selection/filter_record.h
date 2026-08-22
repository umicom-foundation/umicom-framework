/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/filter_record.h
 *
 * PURPOSE:
 *   Define the reusable selection filter record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FILTER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_FILTER_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionFilterRecord {
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
} UmiWorkbenchSelectionFilterRecord;

void umi_workbench_selection_filter_record_init(
    UmiWorkbenchSelectionFilterRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_filter_record_validate(
    const UmiWorkbenchSelectionFilterRecord *record);
UmiStatus umi_workbench_selection_filter_record_set_source(
    UmiWorkbenchSelectionFilterRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_filter_record_set_subject(
    UmiWorkbenchSelectionFilterRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_filter_record_set_secondary(
    UmiWorkbenchSelectionFilterRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_filter_record_set_group(
    UmiWorkbenchSelectionFilterRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_filter_record_set_label(
    UmiWorkbenchSelectionFilterRecord *record,
    const char *label);
uint64_t umi_workbench_selection_filter_record_hash(
    const UmiWorkbenchSelectionFilterRecord *record);
void umi_workbench_selection_filter_record_touch(
    UmiWorkbenchSelectionFilterRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
