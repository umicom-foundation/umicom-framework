/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/snapshot_record.h
 *
 * PURPOSE:
 *   Define the reusable selection snapshot record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SNAPSHOT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_SNAPSHOT_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionSnapshotRecord {
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
} UmiWorkbenchSelectionSnapshotRecord;

void umi_workbench_selection_snapshot_record_init(
    UmiWorkbenchSelectionSnapshotRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_snapshot_record_validate(
    const UmiWorkbenchSelectionSnapshotRecord *record);
UmiStatus umi_workbench_selection_snapshot_record_set_source(
    UmiWorkbenchSelectionSnapshotRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_snapshot_record_set_subject(
    UmiWorkbenchSelectionSnapshotRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_snapshot_record_set_secondary(
    UmiWorkbenchSelectionSnapshotRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_snapshot_record_set_group(
    UmiWorkbenchSelectionSnapshotRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_snapshot_record_set_label(
    UmiWorkbenchSelectionSnapshotRecord *record,
    const char *label);
uint64_t umi_workbench_selection_snapshot_record_hash(
    const UmiWorkbenchSelectionSnapshotRecord *record);
void umi_workbench_selection_snapshot_record_touch(
    UmiWorkbenchSelectionSnapshotRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
