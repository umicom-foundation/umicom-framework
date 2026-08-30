/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/metric_record.h
 *
 * PURPOSE:
 *   Define the reusable selection metric record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_METRIC_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_METRIC_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionMetricRecord {
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
} UmiWorkbenchSelectionMetricRecord;

void umi_workbench_selection_metric_record_init(
    UmiWorkbenchSelectionMetricRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_metric_record_validate(
    const UmiWorkbenchSelectionMetricRecord *record);
UmiStatus umi_workbench_selection_metric_record_set_source(
    UmiWorkbenchSelectionMetricRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_metric_record_set_subject(
    UmiWorkbenchSelectionMetricRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_metric_record_set_secondary(
    UmiWorkbenchSelectionMetricRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_metric_record_set_group(
    UmiWorkbenchSelectionMetricRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_metric_record_set_label(
    UmiWorkbenchSelectionMetricRecord *record,
    const char *label);
uint64_t umi_workbench_selection_metric_record_hash(
    const UmiWorkbenchSelectionMetricRecord *record);
void umi_workbench_selection_metric_record_touch(
    UmiWorkbenchSelectionMetricRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
