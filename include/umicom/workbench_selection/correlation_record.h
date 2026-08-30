/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/correlation_record.h
 *
 * PURPOSE:
 *   Define the reusable selection correlation record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_CORRELATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_CORRELATION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionCorrelationRecord {
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
} UmiWorkbenchSelectionCorrelationRecord;

void umi_workbench_selection_correlation_record_init(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_correlation_record_validate(
    const UmiWorkbenchSelectionCorrelationRecord *record);
UmiStatus umi_workbench_selection_correlation_record_set_source(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_correlation_record_set_subject(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_correlation_record_set_secondary(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_correlation_record_set_group(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_correlation_record_set_label(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *label);
uint64_t umi_workbench_selection_correlation_record_hash(
    const UmiWorkbenchSelectionCorrelationRecord *record);
void umi_workbench_selection_correlation_record_touch(
    UmiWorkbenchSelectionCorrelationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
