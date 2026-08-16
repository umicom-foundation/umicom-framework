/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_edit.h
 *
 * PURPOSE:
 *   Define validated, conflict-aware, provider-neutral text edit sets shared
 *   by rename, refactoring, code actions and future AI-assisted transformations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_EDIT_H
#define UMICOM_EDITOR_WORKSPACE_EDIT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"
#include "umicom/editor/text_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_EDIT_API_VERSION 1U
#define UMI_EDITOR_WORKSPACE_EDIT_ID_CAPACITY 128U
#define UMI_EDITOR_WORKSPACE_EDIT_TEXT_CAPACITY 512U
#define UMI_EDITOR_WORKSPACE_EDIT_PROVIDER_CAPACITY 128U

typedef enum UmiEditorWorkspaceEditState {
    UMI_EDITOR_WORKSPACE_EDIT_READY = 1,
    UMI_EDITOR_WORKSPACE_EDIT_APPLIED = 2,
    UMI_EDITOR_WORKSPACE_EDIT_CONFLICT = 3,
    UMI_EDITOR_WORKSPACE_EDIT_SKIPPED = 4
} UmiEditorWorkspaceEditState;

typedef struct UmiEditorWorkspaceTextEdit {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_WORKSPACE_EDIT_ID_CAPACITY];
    char provider_id[UMI_EDITOR_WORKSPACE_EDIT_PROVIDER_CAPACITY];
    UmiEditorSourceLocation location;
    char expected_text[UMI_EDITOR_WORKSPACE_EDIT_TEXT_CAPACITY];
    char replacement_text[UMI_EDITOR_WORKSPACE_EDIT_TEXT_CAPACITY];
    UmiEditorWorkspaceEditState state;
    int required;
} UmiEditorWorkspaceTextEdit;

typedef struct UmiEditorWorkspaceEditSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t edit_count;
    size_t document_count;
    size_t conflict_count;
    size_t applied_count;
    uint64_t revision;
    int finalized;
    int applicable;
} UmiEditorWorkspaceEditSnapshot;

typedef struct UmiEditorWorkspaceEditSet UmiEditorWorkspaceEditSet;

UmiStatus umi_editor_workspace_edit_set_create(
    UmiEditorWorkspaceEditSet **out_edit_set);
void umi_editor_workspace_edit_set_destroy(UmiEditorWorkspaceEditSet *edit_set);
UmiStatus umi_editor_workspace_edit_set_clear(
    UmiEditorWorkspaceEditSet *edit_set);
UmiStatus umi_editor_workspace_edit_set_upsert(
    UmiEditorWorkspaceEditSet *edit_set,
    const UmiEditorWorkspaceTextEdit *edit);
UmiStatus umi_editor_workspace_edit_set_remove(
    UmiEditorWorkspaceEditSet *edit_set,
    const char *edit_id);
UmiStatus umi_editor_workspace_edit_set_finalize(
    UmiEditorWorkspaceEditSet *edit_set);
UmiStatus umi_editor_workspace_edit_set_apply_document(
    UmiEditorWorkspaceEditSet *edit_set,
    const char *document_uri,
    UmiEditorTextBuffer *buffer,
    int require_matching_revision,
    size_t *out_applied_count);
UmiStatus umi_editor_workspace_edit_set_at(
    const UmiEditorWorkspaceEditSet *edit_set,
    size_t index,
    UmiEditorWorkspaceTextEdit *out_edit);
UmiStatus umi_editor_workspace_edit_set_snapshot(
    const UmiEditorWorkspaceEditSet *edit_set,
    UmiEditorWorkspaceEditSnapshot *out_snapshot);
size_t umi_editor_workspace_edit_set_count(
    const UmiEditorWorkspaceEditSet *edit_set);
uint64_t umi_editor_workspace_edit_set_revision(
    const UmiEditorWorkspaceEditSet *edit_set);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_EDIT_H */
