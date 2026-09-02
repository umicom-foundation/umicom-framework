/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_search_orchestration.c
 *
 * PURPOSE:
 *   Implement one auditable state machine for search, preview, planning and
 *   replacement transaction operations across every Umicom frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_search_orchestration.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorWorkspaceSearchOrchestration {
    UmiEditorWorkspaceSearchIndex *index;
    UmiEditorWorkspaceSearchExclusionSet *exclusions;
    UmiEditorWorkspaceSearchPattern *pattern;
    UmiEditorWorkspaceSearchQuery *query;
    UmiEditorWorkspaceReplacementPreview *preview;
    UmiEditorWorkspaceReplacePlan *plan;
    UmiEditorWorkspaceReplaceTransaction *transaction;
    UmiEditorWorkspaceSearchOrchestrationState state;
    UmiStatus last_status;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the record status operation used by this module and its client applications. */
static UmiStatus record_status(
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    UmiStatus status,
    UmiEditorWorkspaceSearchOrchestrationState success_state)
{
    orchestration->last_status = status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        orchestration->state = success_state;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_CANCELLED) {
        orchestration->state =
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_CANCELLED;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_INVALID_STATE ||
               status == UMI_STATUS_BUSY) {
        orchestration->state =
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_CONFLICT;
    }
    orchestration->revision = next_revision(orchestration->revision);
    return status;
}

/*
 * Initialise editor workspace search orchestration from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_workspace_search_orchestration_create(
    const UmiEditorWorkspaceSearchIndexConfig *index_config,
    UmiEditorWorkspaceSearchOrchestration **out_orchestration)
{
    UmiEditorWorkspaceSearchOrchestration *orchestration;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_orchestration = NULL;
    orchestration = (UmiEditorWorkspaceSearchOrchestration *)calloc(
        1U, sizeof(*orchestration));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_workspace_search_index_create(
        index_config, &orchestration->index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_workspace_search_exclusion_set_create(
        &orchestration->exclusions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_workspace_search_pattern_create(
        &orchestration->pattern);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_workspace_search_query_create(&orchestration->query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_workspace_replacement_preview_create(
        &orchestration->preview);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_workspace_replace_plan_create(&orchestration->plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_workspace_replace_transaction_create(
        &orchestration->transaction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    orchestration->state = UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_EMPTY;
    orchestration->last_status = UMI_STATUS_OK;
    orchestration->revision = 1U;
    *out_orchestration = orchestration;
    return UMI_STATUS_OK;

fail:
    umi_editor_workspace_search_orchestration_destroy(orchestration);
    return status;
}

/*
 * Release or reset state held by editor workspace search orchestration so the same storage
 * can be reused safely.
 */
void umi_editor_workspace_search_orchestration_destroy(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return;
    umi_editor_workspace_replace_transaction_destroy(
        orchestration->transaction);
    umi_editor_workspace_replace_plan_destroy(orchestration->plan);
    umi_editor_workspace_replacement_preview_destroy(orchestration->preview);
    umi_editor_workspace_search_query_destroy(orchestration->query);
    umi_editor_workspace_search_pattern_destroy(orchestration->pattern);
    umi_editor_workspace_search_exclusion_set_destroy(
        orchestration->exclusions);
    umi_editor_workspace_search_index_destroy(orchestration->index);
    free(orchestration);
}

/*
 * Provide the editor workspace search orchestration search operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_search_orchestration_search(
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    const UmiEditorWorkspaceSearchPatternRequest *pattern_request,
    const UmiEditorWorkspaceSearchQueryRequest *query_request,
    UmiEditorWorkspaceSearchPatternDiagnostic *out_diagnostic)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || pattern_request == NULL ||
        query_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_search_pattern_compile(
        orchestration->pattern, pattern_request, out_diagnostic);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return record_status(orchestration, status,
                             UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_EMPTY);
    }
    status = umi_editor_workspace_search_query_execute(
        orchestration->query,
        orchestration->index,
        orchestration->pattern,
        orchestration->exclusions,
        query_request);
    return record_status(orchestration,
                         status,
                         UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_SEARCHED);
}

/*
 * Provide the editor workspace search orchestration preview operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_search_orchestration_preview(
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    const UmiEditorWorkspaceReplacementRequest *replacement_request)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || replacement_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state !=
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_SEARCHED &&
        orchestration->state !=
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PREVIEWED) {
        return record_status(orchestration,
                             UMI_STATUS_INVALID_STATE,
                             orchestration->state);
    }
    status = umi_editor_workspace_replacement_preview_build(
        orchestration->preview,
        orchestration->index,
        orchestration->query,
        replacement_request);
    return record_status(orchestration,
                         status,
                         UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PREVIEWED);
}

/*
 * Provide the editor workspace search orchestration plan operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_workspace_search_orchestration_plan(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state !=
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PREVIEWED &&
        orchestration->state !=
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PLANNED &&
        orchestration->state !=
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_CONFLICT) {
        return record_status(orchestration,
                             UMI_STATUS_INVALID_STATE,
                             orchestration->state);
    }
    status = umi_editor_workspace_replace_plan_build(
        orchestration->plan,
        orchestration->preview,
        orchestration->index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        UmiEditorWorkspaceReplacePlanSnapshot snapshot;
        status = umi_editor_workspace_replace_plan_snapshot(
            orchestration->plan, &snapshot);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && !snapshot.applicable) {
            status = UMI_STATUS_INVALID_STATE;
        }
    }
    return record_status(orchestration,
                         status,
                         UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PLANNED);
}

/*
 * Provide the editor workspace search orchestration prepare operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_search_orchestration_prepare(
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || documents == NULL || document_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state !=
        UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PLANNED) {
        return record_status(orchestration,
                             UMI_STATUS_INVALID_STATE,
                             orchestration->state);
    }
    status = umi_editor_workspace_replace_transaction_prepare(
        orchestration->transaction,
        orchestration->plan,
        documents,
        document_count);
    return record_status(orchestration,
                         status,
                         UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PREFLIGHTED);
}

/*
 * Provide the editor workspace search orchestration commit operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_search_orchestration_commit(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state !=
        UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PREFLIGHTED) {
        return record_status(orchestration,
                             UMI_STATUS_INVALID_STATE,
                             orchestration->state);
    }
    status = umi_editor_workspace_replace_transaction_commit(
        orchestration->transaction);
    return record_status(orchestration,
                         status,
                         UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_COMMITTED);
}

/*
 * Provide the editor workspace search orchestration cancel operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_search_orchestration_cancel(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state ==
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PREFLIGHTED ||
        orchestration->state ==
            UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_CONFLICT) {
        status = umi_editor_workspace_replace_transaction_cancel(
            orchestration->transaction);
    }
    orchestration->last_status = status;
    orchestration->state =
        UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_CANCELLED;
    orchestration->revision = next_revision(orchestration->revision);
    return status;
}

/*
 * Provide the editor workspace search orchestration clear results operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_workspace_search_orchestration_clear_results(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_workspace_search_query_clear(orchestration->query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_replacement_preview_clear(
        orchestration->preview);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_replace_plan_clear(orchestration->plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_search_pattern_reset(orchestration->pattern);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    orchestration->last_status = UMI_STATUS_OK;
    orchestration->state = UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_EMPTY;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace search orchestration snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_search_orchestration_snapshot(
    const UmiEditorWorkspaceSearchOrchestration *orchestration,
    UmiEditorWorkspaceSearchOrchestrationSnapshot *out_snapshot)
{
    UmiEditorWorkspaceSearchIndexSnapshot index_snapshot;
    UmiEditorWorkspaceSearchPatternSnapshot pattern_snapshot;
    UmiEditorWorkspaceSearchQuerySnapshot query_snapshot;
    UmiEditorWorkspaceReplacementPreviewSnapshot preview_snapshot;
    UmiEditorWorkspaceReplacePlanSnapshot plan_snapshot;
    UmiEditorWorkspaceReplaceTransactionSnapshot transaction_snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_API_VERSION;
    out_snapshot->state = orchestration->state;
    out_snapshot->last_status = orchestration->last_status;
    out_snapshot->revision = orchestration->revision;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_search_index_snapshot(
            orchestration->index, &index_snapshot) == UMI_STATUS_OK) {
        out_snapshot->indexed_document_count = index_snapshot.document_count;
        out_snapshot->index_revision = index_snapshot.revision;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_search_pattern_snapshot(
            orchestration->pattern, &pattern_snapshot) == UMI_STATUS_OK) {
        out_snapshot->pattern_revision = pattern_snapshot.revision;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_search_query_snapshot(
            orchestration->query, &query_snapshot) == UMI_STATUS_OK) {
        out_snapshot->result_count = query_snapshot.result_count;
        out_snapshot->selected_result_count =
            query_snapshot.selected_result_count;
        out_snapshot->query_revision = query_snapshot.revision;
        out_snapshot->search_complete = query_snapshot.complete;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_replacement_preview_snapshot(
            orchestration->preview, &preview_snapshot) == UMI_STATUS_OK) {
        out_snapshot->preview_item_count = preview_snapshot.item_count;
        out_snapshot->preview_revision = preview_snapshot.revision;
        out_snapshot->preview_ready = preview_snapshot.ready;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_replace_plan_snapshot(
            orchestration->plan, &plan_snapshot) == UMI_STATUS_OK) {
        out_snapshot->planned_edit_count = plan_snapshot.edit_count;
        out_snapshot->plan_revision = plan_snapshot.revision;
        out_snapshot->plan_applicable = plan_snapshot.applicable;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_workspace_replace_transaction_snapshot(
            orchestration->transaction,
            &transaction_snapshot) == UMI_STATUS_OK) {
        out_snapshot->transaction_applied_edit_count =
            transaction_snapshot.applied_edit_count;
        out_snapshot->transaction_revision = transaction_snapshot.revision;
        out_snapshot->transaction_applicable =
            transaction_snapshot.applicable;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace search orchestration index operation used by this module
 * and its client applications.
 */
UmiEditorWorkspaceSearchIndex *umi_editor_workspace_search_orchestration_index(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->index : NULL;
}

/*
 * Provide the editor workspace search orchestration exclusions operation used by this
 * module and its client applications.
 */
UmiEditorWorkspaceSearchExclusionSet *
umi_editor_workspace_search_orchestration_exclusions(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->exclusions : NULL;
}

/*
 * Provide the editor workspace search orchestration query operation used by this module
 * and its client applications.
 */
UmiEditorWorkspaceSearchQuery *umi_editor_workspace_search_orchestration_query(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->query : NULL;
}

/*
 * Provide the editor workspace search orchestration replacement preview operation used by
 * this module and its client applications.
 */
UmiEditorWorkspaceReplacementPreview *
umi_editor_workspace_search_orchestration_replacement_preview(
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->preview : NULL;
}

/*
 * Provide the editor workspace search orchestration replace plan operation used by this
 * module and its client applications.
 */
const UmiEditorWorkspaceReplacePlan *
umi_editor_workspace_search_orchestration_replace_plan(
    const UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->plan : NULL;
}

/*
 * Provide the editor workspace search orchestration revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_workspace_search_orchestration_revision(
    const UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->revision : 0U;
}
