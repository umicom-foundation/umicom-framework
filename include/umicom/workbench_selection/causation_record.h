/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/causation_record.h
 *
 * PURPOSE:
 *   Define the reusable selection causation record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_CAUSATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_CAUSATION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionCausationRecord {
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
} UmiWorkbenchSelectionCausationRecord;

void umi_workbench_selection_causation_record_init(
    UmiWorkbenchSelectionCausationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_causation_record_validate(
    const UmiWorkbenchSelectionCausationRecord *record);
UmiStatus umi_workbench_selection_causation_record_set_source(
    UmiWorkbenchSelectionCausationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_causation_record_set_subject(
    UmiWorkbenchSelectionCausationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_causation_record_set_secondary(
    UmiWorkbenchSelectionCausationRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_causation_record_set_group(
    UmiWorkbenchSelectionCausationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_causation_record_set_label(
    UmiWorkbenchSelectionCausationRecord *record,
    const char *label);
uint64_t umi_workbench_selection_causation_record_hash(
    const UmiWorkbenchSelectionCausationRecord *record);
void umi_workbench_selection_causation_record_touch(
    UmiWorkbenchSelectionCausationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
