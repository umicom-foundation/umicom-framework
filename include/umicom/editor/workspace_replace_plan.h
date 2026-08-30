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

typedef struct UmiEditorWorkspaceReplacePlan UmiEditorWorkspaceReplacePlan;

UmiStatus umi_editor_workspace_replace_plan_create(
    UmiEditorWorkspaceReplacePlan **out_plan);
void umi_editor_workspace_replace_plan_destroy(
    UmiEditorWorkspaceReplacePlan *plan);
UmiStatus umi_editor_workspace_replace_plan_clear(
    UmiEditorWorkspaceReplacePlan *plan);

UmiStatus umi_editor_workspace_replace_plan_build(
    UmiEditorWorkspaceReplacePlan *plan,
    const UmiEditorWorkspaceReplacementPreview *preview,
    const UmiEditorWorkspaceSearchIndex *index);
UmiStatus umi_editor_workspace_replace_plan_at(
    const UmiEditorWorkspaceReplacePlan *plan,
    size_t position,
    UmiEditorWorkspaceTextEdit *out_edit);
UmiStatus umi_editor_workspace_replace_plan_snapshot(
    const UmiEditorWorkspaceReplacePlan *plan,
    UmiEditorWorkspaceReplacePlanSnapshot *out_snapshot);

const UmiEditorWorkspaceEditSet *umi_editor_workspace_replace_plan_edit_set(
    const UmiEditorWorkspaceReplacePlan *plan);
size_t umi_editor_workspace_replace_plan_count(
    const UmiEditorWorkspaceReplacePlan *plan);
uint64_t umi_editor_workspace_replace_plan_revision(
    const UmiEditorWorkspaceReplacePlan *plan);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_REPLACE_PLAN_H */
