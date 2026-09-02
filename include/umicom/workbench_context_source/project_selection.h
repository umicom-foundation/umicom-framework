/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/project_selection.h
 *
 * PURPOSE:
 *   Define the reusable project selection snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PROJECT_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PROJECT_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source project selection data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextSourceProjectSelection {
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
} UmiWorkbenchContextSourceProjectSelection;

/**
 * Initialise workbench context source project selection from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_project_selection_init(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *record_id);
/**
 * Check that workbench context source project selection satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_project_selection_validate(
    const UmiWorkbenchContextSourceProjectSelection *record);
/**
 * Provide the workbench context source project selection set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_project_selection_set_source(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *source_id);
/**
 * Provide the workbench context source project selection set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_project_selection_set_panel(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *panel_id);
/**
 * Provide the workbench context source project selection set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_project_selection_set_subject(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *subject_id);
/**
 * Provide the workbench context source project selection set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_project_selection_set_group(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *group_id);
/**
 * Provide the workbench context source project selection set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_project_selection_set_label(
    UmiWorkbenchContextSourceProjectSelection *record,
    const char *label);
/**
 * Provide the workbench context source project selection hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_source_project_selection_hash(
    const UmiWorkbenchContextSourceProjectSelection *record);
/**
 * Provide the workbench context source project selection touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_source_project_selection_touch(
    UmiWorkbenchContextSourceProjectSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
