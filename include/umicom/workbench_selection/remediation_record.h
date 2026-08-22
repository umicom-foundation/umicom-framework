/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/remediation_record.h
 *
 * PURPOSE:
 *   Define the reusable selection remediation record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_REMEDIATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_REMEDIATION_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionRemediationRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionRemediationRecord;

void umi_workbench_selection_remediation_record_init(
    UmiWorkbenchSelectionRemediationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_remediation_record_validate(
    const UmiWorkbenchSelectionRemediationRecord *record);
UmiStatus umi_workbench_selection_remediation_record_set_source(
    UmiWorkbenchSelectionRemediationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_remediation_record_set_subject(
    UmiWorkbenchSelectionRemediationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_remediation_record_set_related(
    UmiWorkbenchSelectionRemediationRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_remediation_record_set_group(
    UmiWorkbenchSelectionRemediationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_remediation_record_set_description(
    UmiWorkbenchSelectionRemediationRecord *record,
    const char *description);
uint64_t umi_workbench_selection_remediation_record_hash(
    const UmiWorkbenchSelectionRemediationRecord *record);
void umi_workbench_selection_remediation_record_touch(
    UmiWorkbenchSelectionRemediationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
