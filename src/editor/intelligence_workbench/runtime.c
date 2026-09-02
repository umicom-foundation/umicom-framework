/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/runtime.c
 *
 * PURPOSE:
 *   Implement the operational editor-intelligence workbench composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/runtime.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorIntelWorkbenchRuntime {
    UmiEditorWorkspaceSearchOrchestration *search;
    UmiEditorNavigationInsights *navigation;
    UmiEditorCodeActionOrchestration *code_actions;
    UmiEditorIntelSearchProjection search_projection;
    UmiEditorIntelNavigationProjection navigation_projection;
    UmiEditorIntelCodeActionProjection code_action_projection;
    UmiEditorIntelRefactorProjection refactor_projection;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/*
 * Initialise editor intel workbench runtime from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_workbench_runtime_create(
    const UmiEditorWorkspaceSearchIndexConfig *search_config,
    UmiEditorIntelWorkbenchRuntime **out_runtime)
{
    UmiEditorWorkspaceSearchIndexConfig default_config;
    UmiEditorIntelWorkbenchRuntime *runtime;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (search_config == NULL) {
        umi_editor_workspace_search_index_config_init(&default_config);
        search_config = &default_config;
    }
    runtime = (UmiEditorIntelWorkbenchRuntime *)calloc(1U, sizeof(*runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_workspace_search_orchestration_create(
        search_config, &runtime->search);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_navigation_insights_create(&runtime->navigation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_code_action_orchestration_create(
        &runtime->code_actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_intel_search_projection_init(
        &runtime->search_projection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_intel_navigation_projection_init(
        &runtime->navigation_projection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_intel_code_action_projection_init(
        &runtime->code_action_projection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_intel_refactor_projection_init(
        &runtime->refactor_projection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    runtime->revision = 1U;
    *out_runtime = runtime;
    return UMI_STATUS_OK;

fail:
    umi_editor_intel_workbench_runtime_destroy(runtime);
    return status;
}

/*
 * Release or reset state held by editor intel workbench runtime so the same storage can be
 * reused safely.
 */
void umi_editor_intel_workbench_runtime_destroy(
    UmiEditorIntelWorkbenchRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return;
    umi_editor_code_action_orchestration_destroy(runtime->code_actions);
    umi_editor_navigation_insights_destroy(runtime->navigation);
    umi_editor_workspace_search_orchestration_destroy(runtime->search);
    free(runtime);
}

/*
 * Provide the editor intel workbench runtime search service operation used by this module
 * and its client applications.
 */
UmiEditorWorkspaceSearchOrchestration *
umi_editor_intel_workbench_runtime_search_service(
    UmiEditorIntelWorkbenchRuntime *runtime)
{
    return runtime == NULL ? NULL : runtime->search;
}

/*
 * Provide the editor intel workbench runtime navigation service operation used by this
 * module and its client applications.
 */
UmiEditorNavigationInsights *
umi_editor_intel_workbench_runtime_navigation_service(
    UmiEditorIntelWorkbenchRuntime *runtime)
{
    return runtime == NULL ? NULL : runtime->navigation;
}

/*
 * Provide the editor intel workbench runtime code action service operation used by this
 * module and its client applications.
 */
UmiEditorCodeActionOrchestration *
umi_editor_intel_workbench_runtime_code_action_service(
    UmiEditorIntelWorkbenchRuntime *runtime)
{
    return runtime == NULL ? NULL : runtime->code_actions;
}

/*
 * Provide the editor intel workbench runtime search projection operation used by this
 * module and its client applications.
 */
const UmiEditorIntelSearchProjection *
umi_editor_intel_workbench_runtime_search_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime)
{
    return runtime == NULL ? NULL : &runtime->search_projection;
}

/*
 * Provide the editor intel workbench runtime navigation projection operation used by this
 * module and its client applications.
 */
const UmiEditorIntelNavigationProjection *
umi_editor_intel_workbench_runtime_navigation_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime)
{
    return runtime == NULL ? NULL : &runtime->navigation_projection;
}

/*
 * Provide the editor intel workbench runtime code action projection operation used by this
 * module and its client applications.
 */
const UmiEditorIntelCodeActionProjection *
umi_editor_intel_workbench_runtime_code_action_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime)
{
    return runtime == NULL ? NULL : &runtime->code_action_projection;
}

/*
 * Provide the editor intel workbench runtime refactor projection operation used by this
 * module and its client applications.
 */
const UmiEditorIntelRefactorProjection *
umi_editor_intel_workbench_runtime_refactor_projection(
    const UmiEditorIntelWorkbenchRuntime *runtime)
{
    return runtime == NULL ? NULL : &runtime->refactor_projection;
}

/*
 * Provide the editor intel workbench runtime search operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_search(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorWorkspaceSearchPatternRequest *pattern_request,
    const UmiEditorWorkspaceSearchQueryRequest *query_request,
    UmiEditorWorkspaceSearchPatternDiagnostic *out_diagnostic)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_workspace_search_orchestration_search(
        runtime->search, pattern_request, query_request, out_diagnostic);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_search_projection_refresh(
        &runtime->search_projection, runtime->search);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        runtime->revision = next_revision(runtime->revision);
    }
    return status;
}

/*
 * Provide the editor intel workbench runtime open peek operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_open_peek(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorSourceLocation *anchor,
    const UmiEditorNavigationResultSet *results)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_insights_open_peek(
        runtime->navigation, anchor, results);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_navigation_projection_refresh(
        &runtime->navigation_projection, runtime->navigation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        runtime->revision = next_revision(runtime->revision);
    }
    return status;
}

/*
 * Provide the editor intel workbench runtime open references operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_open_references(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorNavigationResultSet *results)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_insights_open_references(
        runtime->navigation, results);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_navigation_projection_refresh(
        &runtime->navigation_projection, runtime->navigation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        runtime->revision = next_revision(runtime->revision);
    }
    return status;
}

/*
 * Provide the editor intel workbench runtime refresh code actions operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_refresh_code_actions(
    UmiEditorIntelWorkbenchRuntime *runtime)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_intel_code_action_projection_refresh(
        &runtime->code_action_projection, runtime->code_actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_refactor_projection_refresh(
        &runtime->refactor_projection, runtime->code_actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        runtime->revision = next_revision(runtime->revision);
    }
    return status;
}

/*
 * Provide the editor intel workbench runtime prepare refactor operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_prepare_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_code_action_orchestration_prepare_transaction(
        runtime->code_actions, documents, document_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_editor_intel_workbench_runtime_refresh_code_actions(runtime);
}

/*
 * Provide the editor intel workbench runtime commit refactor operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_commit_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_code_action_orchestration_commit_transaction(
        runtime->code_actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_editor_intel_refactor_projection_refresh(
            &runtime->refactor_projection, runtime->code_actions);
        return status;
    }
    status = umi_editor_intel_workbench_runtime_refresh_code_actions(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_editor_intel_refactor_projection_record_history(
        &runtime->refactor_projection, "Committed refactoring");
}

/*
 * Provide the editor intel workbench runtime cancel refactor operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_cancel_refactor(
    UmiEditorIntelWorkbenchRuntime *runtime)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_code_action_orchestration_cancel(runtime->code_actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_workbench_runtime_refresh_code_actions(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_editor_intel_refactor_projection_record_history(
        &runtime->refactor_projection, "Cancelled refactoring");
}

/*
 * Provide the editor intel workbench runtime refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_refresh(
    UmiEditorIntelWorkbenchRuntime *runtime)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_intel_search_projection_refresh(
        &runtime->search_projection, runtime->search);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_navigation_projection_refresh(
        &runtime->navigation_projection, runtime->navigation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_code_action_projection_refresh(
        &runtime->code_action_projection, runtime->code_actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_refactor_projection_refresh(
        &runtime->refactor_projection, runtime->code_actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        runtime->revision = next_revision(runtime->revision);
    }
    return status;
}

/*
 * Provide the editor intel workbench runtime snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_workbench_runtime_snapshot(
    const UmiEditorIntelWorkbenchRuntime *runtime,
    UmiEditorIntelWorkbenchSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_INTEL_WORKBENCH_RUNTIME_API_VERSION;
    out_snapshot->active_navigation_surface =
        runtime->navigation_projection.source_snapshot.active_surface;
    out_snapshot->refactor_phase = runtime->refactor_projection.phase;
    out_snapshot->search_result_count =
        runtime->search_projection.results.count;
    out_snapshot->search_group_count = runtime->search_projection.groups.count;
    out_snapshot->navigation_result_count =
        umi_editor_intel_navigation_projection_count(
            &runtime->navigation_projection);
    out_snapshot->code_action_count = runtime->code_action_projection.menu.count;
    out_snapshot->refactor_preview_count =
        runtime->refactor_projection.preview.count;
    out_snapshot->refactor_history_count =
        runtime->refactor_projection.history.count;
    out_snapshot->revision = runtime->revision;
    out_snapshot->search_truncated = runtime->search_projection.truncated;
    out_snapshot->refactor_conflict = runtime->refactor_projection.has_conflict;
    return UMI_STATUS_OK;
}
