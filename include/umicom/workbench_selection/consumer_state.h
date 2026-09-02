/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/consumer_state.h
 *
 * PURPOSE:
 *   Define the reusable selection consumer state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_CONSUMER_STATE_H
#define UMICOM_WORKBENCH_SELECTION_CONSUMER_STATE_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection consumer state data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionConsumerState {
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
} UmiWorkbenchSelectionConsumerState;

/**
 * Initialise workbench selection consumer state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_consumer_state_init(
    UmiWorkbenchSelectionConsumerState *record,
    const char *record_id);
/**
 * Check that workbench selection consumer state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_consumer_state_validate(
    const UmiWorkbenchSelectionConsumerState *record);
/**
 * Provide the workbench selection consumer state set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_state_set_source(
    UmiWorkbenchSelectionConsumerState *record,
    const char *source_id);
/**
 * Provide the workbench selection consumer state set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_state_set_subject(
    UmiWorkbenchSelectionConsumerState *record,
    const char *subject_id);
/**
 * Provide the workbench selection consumer state set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_state_set_related(
    UmiWorkbenchSelectionConsumerState *record,
    const char *related_id);
/**
 * Provide the workbench selection consumer state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_state_set_group(
    UmiWorkbenchSelectionConsumerState *record,
    const char *group_id);
/**
 * Provide the workbench selection consumer state set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_state_set_description(
    UmiWorkbenchSelectionConsumerState *record,
    const char *description);
/**
 * Provide the workbench selection consumer state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_consumer_state_hash(
    const UmiWorkbenchSelectionConsumerState *record);
/**
 * Provide the workbench selection consumer state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_consumer_state_touch(
    UmiWorkbenchSelectionConsumerState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
