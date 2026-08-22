/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/policy_record.h
 *
 * PURPOSE:
 *   Define the reusable provider policy record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_POLICY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_POLICY_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderPolicyRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderPolicyRecord;

void umi_workbench_selection_provider_policy_record_init(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_policy_record_validate(
    const UmiWorkbenchSelectionProviderPolicyRecord *record);
UmiStatus umi_workbench_selection_provider_policy_record_set_provider(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_policy_record_set_source(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_policy_record_set_subject(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_policy_record_set_related(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_policy_record_set_group(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_policy_record_set_description(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_policy_record_hash(
    const UmiWorkbenchSelectionProviderPolicyRecord *record);
void umi_workbench_selection_provider_policy_record_touch(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
