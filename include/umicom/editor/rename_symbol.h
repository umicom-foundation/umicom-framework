/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/rename_symbol.h
 *
 * PURPOSE:
 *   Plan, preview and apply conflict-safe symbol renames using the Framework
 *   symbol index, provider results and reusable workspace edit engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor rename symbol state values accepted by this public contract.
 */
typedef enum UmiEditorRenameSymbolState {
    UMI_EDITOR_RENAME_SYMBOL_EMPTY = 0,
    UMI_EDITOR_RENAME_SYMBOL_PREPARED = 1,
    UMI_EDITOR_RENAME_SYMBOL_APPLYING = 2,
    UMI_EDITOR_RENAME_SYMBOL_APPLIED = 3,
    UMI_EDITOR_RENAME_SYMBOL_CONFLICT = 4,
    UMI_EDITOR_RENAME_SYMBOL_CANCELLED = 5
} UmiEditorRenameSymbolState;

/**
 * Represent the editor rename symbol request data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorRenameSymbolRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *symbol_id;
    const char *new_name;
    int allow_dollar;
    int allow_utf8_identifier_bytes;
} UmiEditorRenameSymbolRequest;

/**
 * Represent the editor rename symbol snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor rename symbol plan data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorRenameSymbolPlan UmiEditorRenameSymbolPlan;

/**
 * Initialise editor rename symbol plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_rename_symbol_plan_create(
    UmiEditorRenameSymbolPlan **out_plan);
/**
 * Release or reset state held by editor rename symbol plan so the same storage can be
 * reused safely.
 */
void umi_editor_rename_symbol_plan_destroy(UmiEditorRenameSymbolPlan *plan);
/**
 * Provide the editor rename symbol plan prepare operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_rename_symbol_plan_prepare(
    UmiEditorRenameSymbolPlan *plan,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorNavigationResultSet *references,
    const UmiEditorRenameSymbolRequest *request);
/**
 * Provide the editor rename symbol plan apply document operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_rename_symbol_plan_apply_document(
    UmiEditorRenameSymbolPlan *plan,
    const char *document_uri,
    UmiEditorTextBuffer *buffer,
    int require_matching_revision,
    size_t *out_applied_count);
/**
 * Provide the editor rename symbol plan cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_rename_symbol_plan_cancel(
    UmiEditorRenameSymbolPlan *plan);
/**
 * Provide the editor rename symbol plan snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_rename_symbol_plan_snapshot(
    const UmiEditorRenameSymbolPlan *plan,
    UmiEditorRenameSymbolSnapshot *out_snapshot);
/**
 * Provide the editor rename symbol plan edits operation used by this module and its client
 * applications.
 */
UmiEditorWorkspaceEditSet *umi_editor_rename_symbol_plan_edits(
    UmiEditorRenameSymbolPlan *plan);
/**
 * Check that editor rename symbol name satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_editor_rename_symbol_name_validate(
    const char *name,
    int allow_dollar,
    int allow_utf8_identifier_bytes);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_RENAME_SYMBOL_H */
