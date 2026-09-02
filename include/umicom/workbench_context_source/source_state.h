/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/source_state.h
 *
 * PURPOSE:
 *   Define the reusable source runtime state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SOURCE_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SOURCE_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source source state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceSourceState {
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
} UmiWorkbenchContextSourceSourceState;

/**
 * Initialise workbench context source source state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_source_state_init(
    UmiWorkbenchContextSourceSourceState *record,
    const char *record_id);
/**
 * Check that workbench context source source state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_source_state_validate(
    const UmiWorkbenchContextSourceSourceState *record);
/**
 * Provide the workbench context source source state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_state_set_source(
    UmiWorkbenchContextSourceSourceState *record,
    const char *source_id);
/**
 * Provide the workbench context source source state set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_state_set_panel(
    UmiWorkbenchContextSourceSourceState *record,
    const char *panel_id);
/**
 * Provide the workbench context source source state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_state_set_subject(
    UmiWorkbenchContextSourceSourceState *record,
    const char *subject_id);
/**
 * Provide the workbench context source source state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_state_set_group(
    UmiWorkbenchContextSourceSourceState *record,
    const char *group_id);
/**
 * Provide the workbench context source source state set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_source_state_set_label(
    UmiWorkbenchContextSourceSourceState *record,
    const char *label);
/**
 * Provide the workbench context source source state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_source_state_hash(
    const UmiWorkbenchContextSourceSourceState *record);
/**
 * Provide the workbench context source source state touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_source_state_touch(
    UmiWorkbenchContextSourceSourceState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
