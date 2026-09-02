/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/source_commit_mapping.h
 *
 * PURPOSE:
 *   Define the reusable source-control commit mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_COMMIT_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_COMMIT_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider source commit mapping data shared with
 * callers of this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSourceCommitMapping {
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
} UmiWorkbenchSelectionProviderSourceCommitMapping;

/**
 * Initialise workbench selection provider source commit mapping from caller-provided
 * values so later operations receive a known state.
 */
void umi_workbench_selection_provider_source_commit_mapping_init(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *record_id);
/**
 * Check that workbench selection provider source commit mapping satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_source_commit_mapping_validate(
    const UmiWorkbenchSelectionProviderSourceCommitMapping *record);
/**
 * Provide the workbench selection provider source commit mapping set provider operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_provider(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider source commit mapping set source operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_source(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *source_id);
/**
 * Provide the workbench selection provider source commit mapping set subject operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_subject(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider source commit mapping set related operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_related(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *related_id);
/**
 * Provide the workbench selection provider source commit mapping set group operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_group(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *group_id);
/**
 * Provide the workbench selection provider source commit mapping set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_commit_mapping_set_description(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    const char *description);
/**
 * Provide the workbench selection provider source commit mapping hash operation used by
 * this module and its client applications.
 */
uint64_t umi_workbench_selection_provider_source_commit_mapping_hash(
    const UmiWorkbenchSelectionProviderSourceCommitMapping *record);
/**
 * Provide the workbench selection provider source commit mapping touch operation used by
 * this module and its client applications.
 */
void umi_workbench_selection_provider_source_commit_mapping_touch(
    UmiWorkbenchSelectionProviderSourceCommitMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
