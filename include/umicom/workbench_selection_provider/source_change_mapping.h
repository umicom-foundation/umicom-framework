/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/source_change_mapping.h
 *
 * PURPOSE:
 *   Define the reusable source-control change mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_CHANGE_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_CHANGE_MAPPING_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider source change mapping data shared with
 * callers of this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSourceChangeMapping {
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
} UmiWorkbenchSelectionProviderSourceChangeMapping;

/**
 * Initialise workbench selection provider source change mapping from caller-provided
 * values so later operations receive a known state.
 */
void umi_workbench_selection_provider_source_change_mapping_init(
    UmiWorkbenchSelectionProviderSourceChangeMapping *record,
    const char *record_id);
/**
 * Check that workbench selection provider source change mapping satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_source_change_mapping_validate(
    const UmiWorkbenchSelectionProviderSourceChangeMapping *record);
/**
 * Provide the workbench selection provider source change mapping set provider operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_change_mapping_set_provider(
    UmiWorkbenchSelectionProviderSourceChangeMapping *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider source change mapping set source operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_change_mapping_set_source(
    UmiWorkbenchSelectionProviderSourceChangeMapping *record,
    const char *source_id);
/**
 * Provide the workbench selection provider source change mapping set subject operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_change_mapping_set_subject(
    UmiWorkbenchSelectionProviderSourceChangeMapping *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider source change mapping set related operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_change_mapping_set_related(
    UmiWorkbenchSelectionProviderSourceChangeMapping *record,
    const char *related_id);
/**
 * Provide the workbench selection provider source change mapping set group operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_change_mapping_set_group(
    UmiWorkbenchSelectionProviderSourceChangeMapping *record,
    const char *group_id);
/**
 * Provide the workbench selection provider source change mapping set description operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_source_change_mapping_set_description(
    UmiWorkbenchSelectionProviderSourceChangeMapping *record,
    const char *description);
/**
 * Provide the workbench selection provider source change mapping hash operation used by
 * this module and its client applications.
 */
uint64_t umi_workbench_selection_provider_source_change_mapping_hash(
    const UmiWorkbenchSelectionProviderSourceChangeMapping *record);
/**
 * Provide the workbench selection provider source change mapping touch operation used by
 * this module and its client applications.
 */
void umi_workbench_selection_provider_source_change_mapping_touch(
    UmiWorkbenchSelectionProviderSourceChangeMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
