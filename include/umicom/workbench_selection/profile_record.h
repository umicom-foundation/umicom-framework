/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/profile_record.h
 *
 * PURPOSE:
 *   Define the reusable selection profile record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROFILE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROFILE_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProfileRecord {
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
} UmiWorkbenchSelectionProfileRecord;

void umi_workbench_selection_profile_record_init(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_profile_record_validate(
    const UmiWorkbenchSelectionProfileRecord *record);
UmiStatus umi_workbench_selection_profile_record_set_source(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_profile_record_set_subject(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_profile_record_set_secondary(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_profile_record_set_group(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_profile_record_set_label(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *label);
uint64_t umi_workbench_selection_profile_record_hash(
    const UmiWorkbenchSelectionProfileRecord *record);
void umi_workbench_selection_profile_record_touch(
    UmiWorkbenchSelectionProfileRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
