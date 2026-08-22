/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/policy_record.h
 *
 * PURPOSE:
 *   Define the reusable selection policy record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_POLICY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_POLICY_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionPolicyRecord {
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
} UmiWorkbenchSelectionPolicyRecord;

void umi_workbench_selection_policy_record_init(
    UmiWorkbenchSelectionPolicyRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_policy_record_validate(
    const UmiWorkbenchSelectionPolicyRecord *record);
UmiStatus umi_workbench_selection_policy_record_set_source(
    UmiWorkbenchSelectionPolicyRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_policy_record_set_subject(
    UmiWorkbenchSelectionPolicyRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_policy_record_set_secondary(
    UmiWorkbenchSelectionPolicyRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_policy_record_set_group(
    UmiWorkbenchSelectionPolicyRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_policy_record_set_label(
    UmiWorkbenchSelectionPolicyRecord *record,
    const char *label);
uint64_t umi_workbench_selection_policy_record_hash(
    const UmiWorkbenchSelectionPolicyRecord *record);
void umi_workbench_selection_policy_record_touch(
    UmiWorkbenchSelectionPolicyRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
