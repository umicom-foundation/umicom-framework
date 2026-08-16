/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/rename_symbol.h
 *
 * PURPOSE:
 *   Plan, preview and apply conflict-safe symbol renames using the Framework
 *   symbol index, provider results and reusable workspace edit engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_RENAME_SYMBOL_H
#define UMICOM_EDITOR_RENAME_SYMBOL_H

#include "umicom/editor/navigation_result.h"
#include "umicom/editor/symbol_index.h"
#include "umicom/editor/workspace_edit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_RENAME_SYMBOL_API_VERSION 1U
#define UMI_EDITOR_RENAME_NAME_CAPACITY 256U

typedef enum UmiEditorRenameSymbolState {
    UMI_EDITOR_RENAME_SYMBOL_EMPTY = 0,
    UMI_EDITOR_RENAME_SYMBOL_PREPARED = 1,
    UMI_EDITOR_RENAME_SYMBOL_APPLYING = 2,
    UMI_EDITOR_RENAME_SYMBOL_APPLIED = 3,
    UMI_EDITOR_RENAME_SYMBOL_CONFLICT = 4,
    UMI_EDITOR_RENAME_SYMBOL_CANCELLED = 5
} UmiEditorRenameSymbolState;

typedef struct UmiEditorRenameSymbolRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *symbol_id;
    const char *new_name;
    int allow_dollar;
    int allow_utf8_identifier_bytes;
} UmiEditorRenameSymbolRequest;

typedef struct UmiEditorRenameSymbolSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char old_name[UMI_EDITOR_RENAME_NAME_CAPACITY];
    char new_name[UMI_EDITOR_RENAME_NAME_CAPACITY];
    UmiEditorRenameSymbolState state;
    size_t edit_count;
    size_t document_count;
    size_t conflict_count;
    uint64_t revision;
    int can_apply;
} UmiEditorRenameSymbolSnapshot;

typedef struct UmiEditorRenameSymbolPlan UmiEditorRenameSymbolPlan;

UmiStatus umi_editor_rename_symbol_plan_create(
    UmiEditorRenameSymbolPlan **out_plan);
void umi_editor_rename_symbol_plan_destroy(UmiEditorRenameSymbolPlan *plan);
UmiStatus umi_editor_rename_symbol_plan_prepare(
    UmiEditorRenameSymbolPlan *plan,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorNavigationResultSet *references,
    const UmiEditorRenameSymbolRequest *request);
UmiStatus umi_editor_rename_symbol_plan_apply_document(
    UmiEditorRenameSymbolPlan *plan,
    const char *document_uri,
    UmiEditorTextBuffer *buffer,
    int require_matching_revision,
    size_t *out_applied_count);
UmiStatus umi_editor_rename_symbol_plan_cancel(
    UmiEditorRenameSymbolPlan *plan);
UmiStatus umi_editor_rename_symbol_plan_snapshot(
    const UmiEditorRenameSymbolPlan *plan,
    UmiEditorRenameSymbolSnapshot *out_snapshot);
UmiEditorWorkspaceEditSet *umi_editor_rename_symbol_plan_edits(
    UmiEditorRenameSymbolPlan *plan);
UmiStatus umi_editor_rename_symbol_name_validate(
    const char *name,
    int allow_dollar,
    int allow_utf8_identifier_bytes);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_RENAME_SYMBOL_H */
