/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/source_branch_mapping.h
 *
 * PURPOSE:
 *   Define the reusable source-control branch mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_BRANCH_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_BRANCH_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider source branch mapping data shared with
 * callers of this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSourceBranchMapping {
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
} UmiWorkbenchSelectionProviderSourceBranchMapping;

/**
 * Initialise workbench selection provider source branch mapping from caller-provided
 * values so later operations receive a known state.
 */
void umi_workbench_selection_provider_source_branch_mapping_init(
    UmiWorkbenchSelectionProviderSourceBranchMapping *record,
    const char *record_id);
/**
 * Check that workbench selection provider source branch mapping satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_source_branch_mapping_validate(
    const UmiWorkbenchSelectionProviderSourceBranchMapping *record);
/**
 * Provide the workbench selection provider source branch mapping set provider operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_branch_mapping_set_provider(
    UmiWorkbenchSelectionProviderSourceBranchMapping *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider source branch mapping set source operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_branch_mapping_set_source(
    UmiWorkbenchSelectionProviderSourceBranchMapping *record,
    const char *source_id);
/**
 * Provide the workbench selection provider source branch mapping set subject operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_branch_mapping_set_subject(
    UmiWorkbenchSelectionProviderSourceBranchMapping *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider source branch mapping set related operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_branch_mapping_set_related(
    UmiWorkbenchSelectionProviderSourceBranchMapping *record,
    const char *related_id);
/**
 * Provide the workbench selection provider source branch mapping set group operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_branch_mapping_set_group(
    UmiWorkbenchSelectionProviderSourceBranchMapping *record,
    const char *group_id);
/**
 * Provide the workbench selection provider source branch mapping set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_branch_mapping_set_description(
    UmiWorkbenchSelectionProviderSourceBranchMapping *record,
    const char *description);
/**
 * Provide the workbench selection provider source branch mapping hash operation used by
 * this module and its client applications.
 */
uint64_t umi_workbench_selection_provider_source_branch_mapping_hash(
    const UmiWorkbenchSelectionProviderSourceBranchMapping *record);
/**
 * Provide the workbench selection provider source branch mapping touch operation used by
 * this module and its client applications.
 */
void umi_workbench_selection_provider_source_branch_mapping_touch(
    UmiWorkbenchSelectionProviderSourceBranchMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
