/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/mapping_state.h
 *
 * PURPOSE:
 *   Define the reusable mapping runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_MAPPING_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_MAPPING_STATE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider mapping state data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderMappingState {
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
} UmiWorkbenchSelectionProviderMappingState;

/**
 * Initialise workbench selection provider mapping state from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_mapping_state_init(
    UmiWorkbenchSelectionProviderMappingState *record,
    const char *record_id);
/**
 * Check that workbench selection provider mapping state satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_mapping_state_validate(
    const UmiWorkbenchSelectionProviderMappingState *record);
/**
 * Provide the workbench selection provider mapping state set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_mapping_state_set_provider(
    UmiWorkbenchSelectionProviderMappingState *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider mapping state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_mapping_state_set_source(
    UmiWorkbenchSelectionProviderMappingState *record,
    const char *source_id);
/**
 * Provide the workbench selection provider mapping state set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_mapping_state_set_subject(
    UmiWorkbenchSelectionProviderMappingState *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider mapping state set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_mapping_state_set_related(
    UmiWorkbenchSelectionProviderMappingState *record,
    const char *related_id);
/**
 * Provide the workbench selection provider mapping state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_mapping_state_set_group(
    UmiWorkbenchSelectionProviderMappingState *record,
    const char *group_id);
/**
 * Provide the workbench selection provider mapping state set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_mapping_state_set_description(
    UmiWorkbenchSelectionProviderMappingState *record,
    const char *description);
/**
 * Provide the workbench selection provider mapping state hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_mapping_state_hash(
    const UmiWorkbenchSelectionProviderMappingState *record);
/**
 * Provide the workbench selection provider mapping state touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_mapping_state_touch(
    UmiWorkbenchSelectionProviderMappingState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
