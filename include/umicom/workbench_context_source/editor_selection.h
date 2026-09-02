/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/editor_selection.h
 *
 * PURPOSE:
 *   Define the reusable editor selection snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_EDITOR_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_EDITOR_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source editor selection data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceEditorSelection {
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
} UmiWorkbenchContextSourceEditorSelection;

/**
 * Initialise workbench context source editor selection from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_editor_selection_init(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *record_id);
/**
 * Check that workbench context source editor selection satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_editor_selection_validate(
    const UmiWorkbenchContextSourceEditorSelection *record);
/**
 * Provide the workbench context source editor selection set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_selection_set_source(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *source_id);
/**
 * Provide the workbench context source editor selection set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_selection_set_panel(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *panel_id);
/**
 * Provide the workbench context source editor selection set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_selection_set_subject(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *subject_id);
/**
 * Provide the workbench context source editor selection set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_selection_set_group(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *group_id);
/**
 * Provide the workbench context source editor selection set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_editor_selection_set_label(
    UmiWorkbenchContextSourceEditorSelection *record,
    const char *label);
/**
 * Provide the workbench context source editor selection hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_editor_selection_hash(
    const UmiWorkbenchContextSourceEditorSelection *record);
/**
 * Provide the workbench context source editor selection touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_source_editor_selection_touch(
    UmiWorkbenchContextSourceEditorSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
