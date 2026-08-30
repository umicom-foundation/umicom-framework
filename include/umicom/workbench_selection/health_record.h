/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/health_record.h
 *
 * PURPOSE:
 *   Define the reusable selection health record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_HEALTH_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_HEALTH_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionHealthRecord {
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
} UmiWorkbenchSelectionHealthRecord;

void umi_workbench_selection_health_record_init(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_health_record_validate(
    const UmiWorkbenchSelectionHealthRecord *record);
UmiStatus umi_workbench_selection_health_record_set_source(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_health_record_set_subject(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_health_record_set_secondary(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_health_record_set_group(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_health_record_set_label(
    UmiWorkbenchSelectionHealthRecord *record,
    const char *label);
uint64_t umi_workbench_selection_health_record_hash(
    const UmiWorkbenchSelectionHealthRecord *record);
void umi_workbench_selection_health_record_touch(
    UmiWorkbenchSelectionHealthRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
