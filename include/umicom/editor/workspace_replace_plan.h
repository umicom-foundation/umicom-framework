/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_replace_plan.h
 *
 * PURPOSE:
 *   Convert selected replacement-preview items into one deterministic,
 *   conflict-aware Framework workspace edit set. The plan never writes files;
 *   it prepares the complete transaction input for later preflight and commit.
 *
 * OWNERSHIP:
 *   The plan owns its UmiEditorWorkspaceEditSet. The edit-set pointer returned
 *   by edit_set() is borrowed and remains valid until the plan is rebuilt,
 *   cleared or destroyed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_REPLACE_PLAN_H
#define UMICOM_EDITOR_WORKSPACE_REPLACE_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/workspace_edit.h"
#include "umicom/editor/workspace_search_index.h"
#include "umicom/editor/workspace_search_preview.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_REPLACE_PLAN_API_VERSION 1U

/**
 * Represent the editor workspace replace plan snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorWorkspaceReplacePlanSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t selected_item_count;
    size_t edit_count;
    size_t document_count;
    size_t skipped_item_count;
    size_t conflict_item_count;
    uint64_t source_preview_revision;
    uint64_t source_index_revision;
    uint64_t edit_set_revision;
    uint64_t revision;
    int finalized;
    int applicable;
} UmiEditorWorkspaceReplacePlanSnapshot;

/**
 * Represent the editor workspace replace plan data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWorkspaceReplacePlan UmiEditorWorkspaceReplacePlan;

/**
 * Initialise editor workspace replace plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_workspace_replace_plan_create(
    UmiEditorWorkspaceReplacePlan **out_plan);
/**
 * Release or reset state held by editor workspace replace plan so the same storage can be
 * reused safely.
 */
void umi_editor_workspace_replace_plan_destroy(
    UmiEditorWorkspaceReplacePlan *plan);
/**
 * Release or reset state held by editor workspace replace plan so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_workspace_replace_plan_clear(
    UmiEditorWorkspaceReplacePlan *plan);

/**
 * Provide the editor workspace replace plan build operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_workspace_replace_plan_build(
    UmiEditorWorkspaceReplacePlan *plan,
    const UmiEditorWorkspaceReplacementPreview *preview,
    const UmiEditorWorkspaceSearchIndex *index);
/**
 * Find editor workspace replace plan while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_workspace_replace_plan_at(
    const UmiEditorWorkspaceReplacePlan *plan,
    size_t position,
    UmiEditorWorkspaceTextEdit *out_edit);
/**
 * Provide the editor workspace replace plan snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_workspace_replace_plan_snapshot(
    const UmiEditorWorkspaceReplacePlan *plan,
    UmiEditorWorkspaceReplacePlanSnapshot *out_snapshot);

/**
 * Copy editor workspace replace plan edit into module-owned storage so callers keep
 * ownership of their input values.
 */
const UmiEditorWorkspaceEditSet *umi_editor_workspace_replace_plan_edit_set(
    const UmiEditorWorkspaceReplacePlan *plan);
/**
 * Return the number of records represented by editor workspace replace plan without
 * changing their state.
 */
size_t umi_editor_workspace_replace_plan_count(
    const UmiEditorWorkspaceReplacePlan *plan);
/**
 * Provide the editor workspace replace plan revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_workspace_replace_plan_revision(
    const UmiEditorWorkspaceReplacePlan *plan);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_REPLACE_PLAN_H */
