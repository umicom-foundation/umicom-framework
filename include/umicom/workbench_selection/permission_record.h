/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/permission_record.h
 *
 * PURPOSE:
 *   Define the reusable selection permission record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PERMISSION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PERMISSION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionPermissionRecord {
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
} UmiWorkbenchSelectionPermissionRecord;

void umi_workbench_selection_permission_record_init(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_permission_record_validate(
    const UmiWorkbenchSelectionPermissionRecord *record);
UmiStatus umi_workbench_selection_permission_record_set_source(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_permission_record_set_subject(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_permission_record_set_secondary(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_permission_record_set_group(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_permission_record_set_label(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *label);
uint64_t umi_workbench_selection_permission_record_hash(
    const UmiWorkbenchSelectionPermissionRecord *record);
void umi_workbench_selection_permission_record_touch(
    UmiWorkbenchSelectionPermissionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
