/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/focus_state.h
 *
 * PURPOSE:
 *   Define the reusable focus state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_FOCUS_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_FOCUS_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source focus state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceFocusState {
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
} UmiWorkbenchContextSourceFocusState;

/**
 * Initialise workbench context source focus state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_focus_state_init(
    UmiWorkbenchContextSourceFocusState *record,
    const char *record_id);
/**
 * Check that workbench context source focus state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_focus_state_validate(
    const UmiWorkbenchContextSourceFocusState *record);
/**
 * Provide the workbench context source focus state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_focus_state_set_source(
    UmiWorkbenchContextSourceFocusState *record,
    const char *source_id);
/**
 * Provide the workbench context source focus state set panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_focus_state_set_panel(
    UmiWorkbenchContextSourceFocusState *record,
    const char *panel_id);
/**
 * Provide the workbench context source focus state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_focus_state_set_subject(
    UmiWorkbenchContextSourceFocusState *record,
    const char *subject_id);
/**
 * Provide the workbench context source focus state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_focus_state_set_group(
    UmiWorkbenchContextSourceFocusState *record,
    const char *group_id);
/**
 * Provide the workbench context source focus state set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_focus_state_set_label(
    UmiWorkbenchContextSourceFocusState *record,
    const char *label);
/**
 * Provide the workbench context source focus state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_focus_state_hash(
    const UmiWorkbenchContextSourceFocusState *record);
/**
 * Provide the workbench context source focus state touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_source_focus_state_touch(
    UmiWorkbenchContextSourceFocusState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
