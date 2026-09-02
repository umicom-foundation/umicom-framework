/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/provider_state.h
 *
 * PURPOSE:
 *   Define the reusable selection provider state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_STATE_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider state data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionProviderState {
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
} UmiWorkbenchSelectionProviderState;

/**
 * Initialise workbench selection provider state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_state_init(
    UmiWorkbenchSelectionProviderState *record,
    const char *record_id);
/**
 * Check that workbench selection provider state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_provider_state_validate(
    const UmiWorkbenchSelectionProviderState *record);
/**
 * Provide the workbench selection provider state set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_state_set_source(
    UmiWorkbenchSelectionProviderState *record,
    const char *source_id);
/**
 * Provide the workbench selection provider state set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_state_set_subject(
    UmiWorkbenchSelectionProviderState *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider state set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_state_set_related(
    UmiWorkbenchSelectionProviderState *record,
    const char *related_id);
/**
 * Provide the workbench selection provider state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_state_set_group(
    UmiWorkbenchSelectionProviderState *record,
    const char *group_id);
/**
 * Provide the workbench selection provider state set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_state_set_description(
    UmiWorkbenchSelectionProviderState *record,
    const char *description);
/**
 * Provide the workbench selection provider state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_provider_state_hash(
    const UmiWorkbenchSelectionProviderState *record);
/**
 * Provide the workbench selection provider state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_provider_state_touch(
    UmiWorkbenchSelectionProviderState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
