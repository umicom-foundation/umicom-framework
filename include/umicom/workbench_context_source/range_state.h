/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/range_state.h
 *
 * PURPOSE:
 *   Define the reusable selection range state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_RANGE_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_RANGE_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source range state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceRangeState {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceRangeState;

/**
 * Initialise workbench context source range state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_range_state_init(
    UmiWorkbenchContextSourceRangeState *record,
    const char *record_id);
/**
 * Check that workbench context source range state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_range_state_validate(
    const UmiWorkbenchContextSourceRangeState *record);
/**
 * Provide the workbench context source range state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_range_state_set_source(
    UmiWorkbenchContextSourceRangeState *record,
    const char *source_id);
/**
 * Provide the workbench context source range state set panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_range_state_set_panel(
    UmiWorkbenchContextSourceRangeState *record,
    const char *panel_id);
/**
 * Provide the workbench context source range state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_range_state_set_subject(
    UmiWorkbenchContextSourceRangeState *record,
    const char *subject_id);
/**
 * Provide the workbench context source range state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_range_state_set_group(
    UmiWorkbenchContextSourceRangeState *record,
    const char *group_id);
/**
 * Provide the workbench context source range state set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_range_state_set_label(
    UmiWorkbenchContextSourceRangeState *record,
    const char *label);
/**
 * Provide the workbench context source range state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_range_state_hash(
    const UmiWorkbenchContextSourceRangeState *record);
/**
 * Provide the workbench context source range state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_source_range_state_touch(
    UmiWorkbenchContextSourceRangeState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
