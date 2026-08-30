/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_search_orchestration.h
 *
 * PURPOSE:
 *   Coordinate the complete Framework-owned workspace search and replace flow:
 *   indexed corpus -> compiled pattern -> bounded results -> preview -> edit
 *   plan -> atomic transaction. Applications remain composition shells.
 *
 * LIFECYCLE:
 *   1. Populate the borrowed index and exclusion set.
 *   2. Execute a search with a compiled pattern.
 *   3. Build and select replacement previews.
 *   4. Build the edit plan.
 *   5. Preflight working documents and commit atomically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_H
#define UMICOM_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/workspace_replace_plan.h"
#include "umicom/editor/workspace_replace_transaction.h"
#include "umicom/editor/workspace_search_exclusion.h"
#include "umicom/editor/workspace_search_index.h"
#include "umicom/editor/workspace_search_pattern.h"
#include "umicom/editor/workspace_search_preview.h"
#include "umicom/editor/workspace_search_query.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_API_VERSION 1U

typedef enum UmiEditorWorkspaceSearchOrchestrationState {
    UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_EMPTY = 1,
    UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_SEARCHED = 2,
    UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PREVIEWED = 3,
    UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PLANNED = 4,
    UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PREFLIGHTED = 5,
    UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_COMMITTED = 6,
    UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_CANCELLED = 7,
    UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_CONFLICT = 8
} UmiEditorWorkspaceSearchOrchestrationState;

typedef struct UmiEditorWorkspaceSearchOrchestrationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorWorkspaceSearchOrchestrationState state;
    UmiStatus last_status;
    size_t indexed_document_count;
    size_t result_count;
    size_t selected_result_count;
    size_t preview_item_count;
    size_t planned_edit_count;
    size_t transaction_applied_edit_count;
    uint64_t index_revision;
    uint64_t pattern_revision;
    uint64_t query_revision;
    uint64_t preview_revision;
    uint64_t plan_revision;
    uint64_t transaction_revision;
    uint64_t revision;
    int search_complete;
    int preview_ready;
    int plan_applicable;
    int transaction_applicable;
} UmiEditorWorkspaceSearchOrchestrationSnapshot;

typedef struct UmiEditorWorkspaceSearchOrchestration
    UmiEditorWorkspaceSearchOrchestration;

UmiStatus umi_editor_workspace_search_orchestration_create(
    const UmiEditorWorkspaceSearchIndexConfig *index_config,
    UmiEditorWorkspaceSearchOrchestration **out_orchestration);
void umi_editor_workspace_search_orchestration_destroy(
    UmiEditorWorkspaceSearchOrchestration *orchestration);

UmiStatus umi_editor_workspace_search_orchestration_search(
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    const UmiEditorWorkspaceSearchPatternRequest *pattern_request,
    const UmiEditorWorkspaceSearchQueryRequest *query_request,
    UmiEditorWorkspaceSearchPatternDiagnostic *out_diagnostic);
UmiStatus umi_editor_workspace_search_orchestration_preview(
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    const UmiEditorWorkspaceReplacementRequest *replacement_request);
UmiStatus umi_editor_workspace_search_orchestration_plan(
    UmiEditorWorkspaceSearchOrchestration *orchestration);
UmiStatus umi_editor_workspace_search_orchestration_prepare(
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count);
UmiStatus umi_editor_workspace_search_orchestration_commit(
    UmiEditorWorkspaceSearchOrchestration *orchestration);
UmiStatus umi_editor_workspace_search_orchestration_cancel(
    UmiEditorWorkspaceSearchOrchestration *orchestration);
UmiStatus umi_editor_workspace_search_orchestration_clear_results(
    UmiEditorWorkspaceSearchOrchestration *orchestration);
UmiStatus umi_editor_workspace_search_orchestration_snapshot(
    const UmiEditorWorkspaceSearchOrchestration *orchestration,
    UmiEditorWorkspaceSearchOrchestrationSnapshot *out_snapshot);

/** Borrowed component accessors for adapters and Framework-backed views. */
UmiEditorWorkspaceSearchIndex *umi_editor_workspace_search_orchestration_index(
    UmiEditorWorkspaceSearchOrchestration *orchestration);
UmiEditorWorkspaceSearchExclusionSet *
umi_editor_workspace_search_orchestration_exclusions(
    UmiEditorWorkspaceSearchOrchestration *orchestration);
UmiEditorWorkspaceSearchQuery *umi_editor_workspace_search_orchestration_query(
    UmiEditorWorkspaceSearchOrchestration *orchestration);
UmiEditorWorkspaceReplacementPreview *
umi_editor_workspace_search_orchestration_replacement_preview(
    UmiEditorWorkspaceSearchOrchestration *orchestration);
const UmiEditorWorkspaceReplacePlan *
umi_editor_workspace_search_orchestration_replace_plan(
    const UmiEditorWorkspaceSearchOrchestration *orchestration);

uint64_t umi_editor_workspace_search_orchestration_revision(
    const UmiEditorWorkspaceSearchOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_H */
