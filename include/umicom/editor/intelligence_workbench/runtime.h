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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the editor intel workbench snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor intel workbench runtime data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelWorkbenchRuntime UmiEditorIntelWorkbenchRuntime;

/**
 * Initialise editor intel workbench runtime from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_workbench_runtime_create(
    const UmiEditorWorkspaceSearchIndexConfig *search_config,
    UmiEditorIntelWorkbenchRuntime **out_runtime);
/**
 * Release or reset state held by editor intel workbench runtime so the same storage can be
 * reused safely.
 */
void umi_editor_intel_workbench_runtime_destroy(
    UmiEditorIntelWorkbenchRuntime *runtime);

/**
 * Provide the editor intel workbench runtime search service operation used by this module
 * and its client applications.
 */
UmiEditorWorkspaceSearchOrchestration *
umi_editor_intel_workbench_runtime_search_service(
    UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime navigation service operation used by this
 * module and its client applications.
 */
UmiEditorNavigationInsights *
umi_editor_intel_workbench_runtime_navigation_service(
    UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime code action service operation used by this
 * module and its client applications.
 */
UmiEditorCodeActionOrchestration *
umi_editor_intel_workbench_runtime_code_action_service(
    UmiEditorIntelWorkbenchRuntime *runtime);

/**
 * Provide the editor intel workbench runtime search projection operation used by this
 * module and its client applications.
 */
const UmiEditorIntelSearchProjection *
umi_editor_intel_workbench_runtime_search_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime navigation projection operation used by this
 * module and its client applications.
 */
const UmiEditorIntelNavigationProjection *
umi_editor_intel_workbench_runtime_navigation_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime code action projection operation used by this
 * module and its client applications.
 */
const UmiEditorIntelCodeActionProjection *
umi_editor_intel_workbench_runtime_code_action_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime refactor projection operation used by this
 * module and its client applications.
 */
const UmiEditorIntelRefactorProjection *
umi_editor_intel_workbench_runtime_refactor_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime);

/**
 * Provide the editor intel workbench runtime search operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_search(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorWorkspaceSearchPatternRequest *pattern_request,
    const UmiEditorWorkspaceSearchQueryRequest *query_request,
    UmiEditorWorkspaceSearchPatternDiagnostic *out_diagnostic);
/**
 * Provide the editor intel workbench runtime open peek operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_open_peek(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results);
/**
 * Provide the editor intel workbench runtime open references operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_open_references(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorNavigationResultSet *results);
/**
 * Provide the editor intel workbench runtime refresh code actions operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_refresh_code_actions(
    UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime prepare refactor operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_prepare_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count);
/**
 * Provide the editor intel workbench runtime commit refactor operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_commit_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime cancel refactor operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_cancel_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_refresh(
    UmiEditorIntelWorkbenchRuntime *runtime);
/**
 * Provide the editor intel workbench runtime snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_snapshot(
    const UmiEditorIntelWorkbenchRuntime *runtime,
    UmiEditorIntelWorkbenchSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_RUNTIME_H */
