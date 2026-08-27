/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/runtime.h
 *
 * PURPOSE:
 *   Own the canonical editor-search, navigation, code-action and refactoring
 *   coordinators behind one frontend-neutral workbench runtime.
 *
 * ARCHITECTURE:
 *   The runtime composes existing Framework engines and projects their state.
 *   It introduces no second search index, language service, policy system or
 *   edit engine. Studio and all future applications consume the same contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RUNTIME_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RUNTIME_H

#include "umicom/editor/intelligence_workbench/code_action_projection.h"
#include "umicom/editor/intelligence_workbench/navigation_projection.h"
#include "umicom/editor/intelligence_workbench/refactor_projection.h"
#include "umicom/editor/intelligence_workbench/search_projection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTEL_WORKBENCH_RUNTIME_API_VERSION 1U

typedef struct UmiEditorIntelWorkbenchSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationInsightSurface active_navigation_surface;
    UmiEditorIntelPhase refactor_phase;
    size_t search_result_count;
    size_t search_group_count;
    size_t navigation_result_count;
    size_t code_action_count;
    size_t refactor_preview_count;
    size_t refactor_history_count;
    uint64_t revision;
    int search_truncated;
    int refactor_conflict;
} UmiEditorIntelWorkbenchSnapshot;

typedef struct UmiEditorIntelWorkbenchRuntime UmiEditorIntelWorkbenchRuntime;

UmiStatus umi_editor_intel_workbench_runtime_create(
    const UmiEditorWorkspaceSearchIndexConfig *search_config,
    UmiEditorIntelWorkbenchRuntime **out_runtime);
void umi_editor_intel_workbench_runtime_destroy(
    UmiEditorIntelWorkbenchRuntime *runtime);

UmiEditorWorkspaceSearchOrchestration *
umi_editor_intel_workbench_runtime_search_service(
    UmiEditorIntelWorkbenchRuntime *runtime);
UmiEditorNavigationInsights *
umi_editor_intel_workbench_runtime_navigation_service(
    UmiEditorIntelWorkbenchRuntime *runtime);
UmiEditorCodeActionOrchestration *
umi_editor_intel_workbench_runtime_code_action_service(
    UmiEditorIntelWorkbenchRuntime *runtime);

const UmiEditorIntelSearchProjection *
umi_editor_intel_workbench_runtime_search_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime);
const UmiEditorIntelNavigationProjection *
umi_editor_intel_workbench_runtime_navigation_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime);
const UmiEditorIntelCodeActionProjection *
umi_editor_intel_workbench_runtime_code_action_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime);
const UmiEditorIntelRefactorProjection *
umi_editor_intel_workbench_runtime_refactor_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime);

UmiStatus umi_editor_intel_workbench_runtime_search(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorWorkspaceSearchPatternRequest *pattern_request,
    const UmiEditorWorkspaceSearchQueryRequest *query_request,
    UmiEditorWorkspaceSearchPatternDiagnostic *out_diagnostic);
UmiStatus umi_editor_intel_workbench_runtime_open_peek(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results);
UmiStatus umi_editor_intel_workbench_runtime_open_references(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorNavigationResultSet *results);
UmiStatus umi_editor_intel_workbench_runtime_refresh_code_actions(
    UmiEditorIntelWorkbenchRuntime *runtime);
UmiStatus umi_editor_intel_workbench_runtime_prepare_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count);
UmiStatus umi_editor_intel_workbench_runtime_commit_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime);
UmiStatus umi_editor_intel_workbench_runtime_cancel_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime);
UmiStatus umi_editor_intel_workbench_runtime_refresh(
    UmiEditorIntelWorkbenchRuntime *runtime);
UmiStatus umi_editor_intel_workbench_runtime_snapshot(
    const UmiEditorIntelWorkbenchRuntime *runtime,
    UmiEditorIntelWorkbenchSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RUNTIME_H */
