/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/view_state.h
 *
 * PURPOSE:
 *   Define the reusable view state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_VIEW_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_VIEW_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source view state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceViewState {
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
} UmiWorkbenchContextSourceViewState;

/**
 * Initialise workbench context source view state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_view_state_init(
    UmiWorkbenchContextSourceViewState *record,
    const char *record_id);
/**
 * Check that workbench context source view state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_view_state_validate(
    const UmiWorkbenchContextSourceViewState *record);
/**
 * Provide the workbench context source view state set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_view_state_set_source(
    UmiWorkbenchContextSourceViewState *record,
    const char *source_id);
/**
 * Provide the workbench context source view state set panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_view_state_set_panel(
    UmiWorkbenchContextSourceViewState *record,
    const char *panel_id);
/**
 * Provide the workbench context source view state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_view_state_set_subject(
    UmiWorkbenchContextSourceViewState *record,
    const char *subject_id);
/**
 * Provide the workbench context source view state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_view_state_set_group(
    UmiWorkbenchContextSourceViewState *record,
    const char *group_id);
/**
 * Provide the workbench context source view state set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_view_state_set_label(
    UmiWorkbenchContextSourceViewState *record,
    const char *label);
/**
 * Provide the workbench context source view state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_view_state_hash(
    const UmiWorkbenchContextSourceViewState *record);
/**
 * Provide the workbench context source view state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_source_view_state_touch(
    UmiWorkbenchContextSourceViewState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
