/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/problem_selection.h
 *
 * PURPOSE:
 *   Define the reusable problem selection snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PROBLEM_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PROBLEM_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source problem selection data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextSourceProblemSelection {
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
} UmiWorkbenchContextSourceProblemSelection;

/**
 * Initialise workbench context source problem selection from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_problem_selection_init(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *record_id);
/**
 * Check that workbench context source problem selection satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_problem_selection_validate(
    const UmiWorkbenchContextSourceProblemSelection *record);
/**
 * Provide the workbench context source problem selection set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_problem_selection_set_source(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *source_id);
/**
 * Provide the workbench context source problem selection set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_problem_selection_set_panel(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *panel_id);
/**
 * Provide the workbench context source problem selection set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_problem_selection_set_subject(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *subject_id);
/**
 * Provide the workbench context source problem selection set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_problem_selection_set_group(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *group_id);
/**
 * Provide the workbench context source problem selection set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_problem_selection_set_label(
    UmiWorkbenchContextSourceProblemSelection *record,
    const char *label);
/**
 * Provide the workbench context source problem selection hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_source_problem_selection_hash(
    const UmiWorkbenchContextSourceProblemSelection *record);
/**
 * Provide the workbench context source problem selection touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_source_problem_selection_touch(
    UmiWorkbenchContextSourceProblemSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
