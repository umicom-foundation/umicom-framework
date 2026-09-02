/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/selection_state.h
 *
 * PURPOSE:
 *   Define the reusable selection runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SELECTION_STATE_H
#define UMICOM_WORKBENCH_SELECTION_SELECTION_STATE_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection selection state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionSelectionState {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionSelectionState;

/**
 * Initialise workbench selection selection state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_selection_state_init(
    UmiWorkbenchSelectionSelectionState *record,
    const char *record_id);
/**
 * Check that workbench selection selection state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_selection_state_validate(
    const UmiWorkbenchSelectionSelectionState *record);
/**
 * Provide the workbench selection selection state set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_selection_state_set_source(
    UmiWorkbenchSelectionSelectionState *record,
    const char *source_id);
/**
 * Provide the workbench selection selection state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_selection_state_set_subject(
    UmiWorkbenchSelectionSelectionState *record,
    const char *subject_id);
/**
 * Provide the workbench selection selection state set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_selection_state_set_secondary(
    UmiWorkbenchSelectionSelectionState *record,
    const char *secondary_id);
/**
 * Provide the workbench selection selection state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_selection_state_set_group(
    UmiWorkbenchSelectionSelectionState *record,
    const char *group_id);
/**
 * Provide the workbench selection selection state set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_selection_state_set_label(
    UmiWorkbenchSelectionSelectionState *record,
    const char *label);
/**
 * Provide the workbench selection selection state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_selection_state_hash(
    const UmiWorkbenchSelectionSelectionState *record);
/**
 * Provide the workbench selection selection state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_selection_state_touch(
    UmiWorkbenchSelectionSelectionState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
