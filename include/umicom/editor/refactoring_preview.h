/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/refactoring_preview.h
 *
 * PURPOSE:
 *   Define selectable, conflict-aware previews for Framework refactoring
 *   plans before any document is modified.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_REFACTORING_PREVIEW_H
#define UMICOM_EDITOR_REFACTORING_PREVIEW_H

#include "umicom/editor/refactoring_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_REFACTORING_PREVIEW_API_VERSION 1U

/**
 * Represent the editor refactoring preview item data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorRefactoringPreviewItem {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorWorkspaceTextEdit edit;
    int included;
    int selectable;
    int conflict;
} UmiEditorRefactoringPreviewItem;

/**
 * Represent the editor refactoring preview snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorRefactoringPreviewSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char plan_id[UMI_EDITOR_REFACTORING_ID_CAPACITY];
    size_t item_count;
    size_t document_count;
    size_t included_count;
    size_t excluded_count;
    size_t required_count;
    size_t conflict_count;
    uint64_t plan_revision;
    uint64_t revision;
    int ready;
} UmiEditorRefactoringPreviewSnapshot;

/**
 * Represent the editor refactoring preview data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorRefactoringPreview UmiEditorRefactoringPreview;

/**
 * Initialise editor refactoring preview from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_refactoring_preview_create(
    UmiEditorRefactoringPreview **out_preview);
/**
 * Release or reset state held by editor refactoring preview so the same storage can be
 * reused safely.
 */
void umi_editor_refactoring_preview_destroy(
    UmiEditorRefactoringPreview *preview);
/**
 * Provide the editor refactoring preview build operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_preview_build(
    UmiEditorRefactoringPreview *preview,
    const UmiEditorRefactoringPlan *plan);
/**
 * Provide the editor refactoring preview set included operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_refactoring_preview_set_included(
    UmiEditorRefactoringPreview *preview,
    const char *edit_id,
    int included);
/**
 * Provide the editor refactoring preview select all operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_preview_select_all(
    UmiEditorRefactoringPreview *preview,
    int included);
/**
 * Find editor refactoring preview while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_refactoring_preview_at(
    const UmiEditorRefactoringPreview *preview,
    size_t index,
    UmiEditorRefactoringPreviewItem *out_item);
/**
 * Provide the editor refactoring preview selected edits operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_refactoring_preview_selected_edits(
    const UmiEditorRefactoringPreview *preview,
    UmiEditorWorkspaceEditSet *out_edit_set);
/**
 * Provide the editor refactoring preview snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_preview_snapshot(
    const UmiEditorRefactoringPreview *preview,
    UmiEditorRefactoringPreviewSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor refactoring preview without changing
 * their state.
 */
size_t umi_editor_refactoring_preview_count(
    const UmiEditorRefactoringPreview *preview);
/**
 * Provide the editor refactoring preview revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_refactoring_preview_revision(
    const UmiEditorRefactoringPreview *preview);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_REFACTORING_PREVIEW_H */
