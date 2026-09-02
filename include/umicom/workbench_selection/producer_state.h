/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/producer_state.h
 *
 * PURPOSE:
 *   Define the reusable selection producer state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PRODUCER_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PRODUCER_STATE_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection producer state data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionProducerState {
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
} UmiWorkbenchSelectionProducerState;

/**
 * Initialise workbench selection producer state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_producer_state_init(
    UmiWorkbenchSelectionProducerState *record,
    const char *record_id);
/**
 * Check that workbench selection producer state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_producer_state_validate(
    const UmiWorkbenchSelectionProducerState *record);
/**
 * Provide the workbench selection producer state set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_producer_state_set_source(
    UmiWorkbenchSelectionProducerState *record,
    const char *source_id);
/**
 * Provide the workbench selection producer state set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_producer_state_set_subject(
    UmiWorkbenchSelectionProducerState *record,
    const char *subject_id);
/**
 * Provide the workbench selection producer state set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_producer_state_set_related(
    UmiWorkbenchSelectionProducerState *record,
    const char *related_id);
/**
 * Provide the workbench selection producer state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_producer_state_set_group(
    UmiWorkbenchSelectionProducerState *record,
    const char *group_id);
/**
 * Provide the workbench selection producer state set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_producer_state_set_description(
    UmiWorkbenchSelectionProducerState *record,
    const char *description);
/**
 * Provide the workbench selection producer state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_producer_state_hash(
    const UmiWorkbenchSelectionProducerState *record);
/**
 * Provide the workbench selection producer state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_producer_state_touch(
    UmiWorkbenchSelectionProducerState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
