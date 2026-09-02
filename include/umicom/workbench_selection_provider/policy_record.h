/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/policy_record.h
 *
 * PURPOSE:
 *   Define the reusable provider policy record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_POLICY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_POLICY_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider policy record data shared with callers of
 * this public contract.
 */
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

/**
 * Initialise workbench selection provider policy record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_policy_record_init(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *record_id);
/**
 * Check that workbench selection provider policy record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_policy_record_validate(
    const UmiWorkbenchSelectionProviderPolicyRecord *record);
/**
 * Provide the workbench selection provider policy record set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_policy_record_set_provider(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider policy record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_policy_record_set_source(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection provider policy record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_policy_record_set_subject(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider policy record set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_policy_record_set_related(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection provider policy record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_policy_record_set_group(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection provider policy record set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_policy_record_set_description(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    const char *description);
/**
 * Provide the workbench selection provider policy record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_policy_record_hash(
    const UmiWorkbenchSelectionProviderPolicyRecord *record);
/**
 * Provide the workbench selection provider policy record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_policy_record_touch(
    UmiWorkbenchSelectionProviderPolicyRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
