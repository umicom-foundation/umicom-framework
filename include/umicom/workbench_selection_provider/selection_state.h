/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/selection_state.h
 *
 * PURPOSE:
 *   Define the reusable provider selection runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SELECTION_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SELECTION_STATE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider selection state data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSelectionState {
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
} UmiWorkbenchSelectionProviderSelectionState;

/**
 * Initialise workbench selection provider selection state from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_selection_state_init(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *record_id);
/**
 * Check that workbench selection provider selection state satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_selection_state_validate(
    const UmiWorkbenchSelectionProviderSelectionState *record);
/**
 * Provide the workbench selection provider selection state set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_selection_state_set_provider(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider selection state set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_selection_state_set_source(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *source_id);
/**
 * Provide the workbench selection provider selection state set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_selection_state_set_subject(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider selection state set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_selection_state_set_related(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *related_id);
/**
 * Provide the workbench selection provider selection state set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_selection_state_set_group(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *group_id);
/**
 * Provide the workbench selection provider selection state set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_selection_state_set_description(
    UmiWorkbenchSelectionProviderSelectionState *record,
    const char *description);
/**
 * Provide the workbench selection provider selection state hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_selection_state_hash(
    const UmiWorkbenchSelectionProviderSelectionState *record);
/**
 * Provide the workbench selection provider selection state touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_selection_state_touch(
    UmiWorkbenchSelectionProviderSelectionState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
