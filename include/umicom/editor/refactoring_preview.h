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

typedef struct UmiEditorRefactoringPreviewItem {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorWorkspaceTextEdit edit;
    int included;
    int selectable;
    int conflict;
} UmiEditorRefactoringPreviewItem;

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

typedef struct UmiEditorRefactoringPreview UmiEditorRefactoringPreview;

UmiStatus umi_editor_refactoring_preview_create(
    UmiEditorRefactoringPreview **out_preview);
void umi_editor_refactoring_preview_destroy(
    UmiEditorRefactoringPreview *preview);
UmiStatus umi_editor_refactoring_preview_build(
    UmiEditorRefactoringPreview *preview,
    const UmiEditorRefactoringPlan *plan);
UmiStatus umi_editor_refactoring_preview_set_included(
    UmiEditorRefactoringPreview *preview,
    const char *edit_id,
    int included);
UmiStatus umi_editor_refactoring_preview_select_all(
    UmiEditorRefactoringPreview *preview,
    int included);
UmiStatus umi_editor_refactoring_preview_at(
    const UmiEditorRefactoringPreview *preview,
    size_t index,
    UmiEditorRefactoringPreviewItem *out_item);
UmiStatus umi_editor_refactoring_preview_selected_edits(
    const UmiEditorRefactoringPreview *preview,
    UmiEditorWorkspaceEditSet *out_edit_set);
UmiStatus umi_editor_refactoring_preview_snapshot(
    const UmiEditorRefactoringPreview *preview,
    UmiEditorRefactoringPreviewSnapshot *out_snapshot);
size_t umi_editor_refactoring_preview_count(
    const UmiEditorRefactoringPreview *preview);
uint64_t umi_editor_refactoring_preview_revision(
    const UmiEditorRefactoringPreview *preview);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_REFACTORING_PREVIEW_H */
