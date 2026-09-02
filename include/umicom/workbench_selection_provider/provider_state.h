/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/provider_state.h
 *
 * PURPOSE:
 *   Define the reusable provider runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROVIDER_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROVIDER_STATE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider provider state data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderProviderState {
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
} UmiWorkbenchSelectionProviderProviderState;

/**
 * Initialise workbench selection provider provider state from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_provider_state_init(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *record_id);
/**
 * Check that workbench selection provider provider state satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_provider_state_validate(
    const UmiWorkbenchSelectionProviderProviderState *record);
/**
 * Provide the workbench selection provider provider state set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_state_set_provider(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider provider state set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_state_set_source(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *source_id);
/**
 * Provide the workbench selection provider provider state set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_state_set_subject(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider provider state set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_state_set_related(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *related_id);
/**
 * Provide the workbench selection provider provider state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_state_set_group(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *group_id);
/**
 * Provide the workbench selection provider provider state set description operation used
 * by this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_provider_state_set_description(
    UmiWorkbenchSelectionProviderProviderState *record,
    const char *description);
/**
 * Provide the workbench selection provider provider state hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_provider_state_hash(
    const UmiWorkbenchSelectionProviderProviderState *record);
/**
 * Provide the workbench selection provider provider state touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_provider_state_touch(
    UmiWorkbenchSelectionProviderProviderState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
