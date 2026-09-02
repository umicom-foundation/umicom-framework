/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/code_action_orchestration.c
 *
 * PURPOSE:
 *   Implement the reusable code-action and refactoring coordination boundary
 *   consumed by Studio and future lightweight Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/code_action_orchestration.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorCodeActionOrchestration {
    UmiEditorCodeActionProviderRegistry *providers;
    UmiEditorCodeActionQuery *query;
    UmiEditorRefactoringPlan *plan;
    UmiEditorRefactoringPreview *preview;
    UmiEditorEditTransaction *transaction;
    UmiEditorWorkspaceEditSet *selected_edits;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the add revision operation used by this module and its client applications. */
static uint64_t add_revision(uint64_t left, uint64_t right)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (UINT64_MAX - left < right) {
        uint64_t mixed = left ^ right;
        return mixed == UINT64_MAX ? 1U : mixed + 1U;
    }
    return left + right;
}

/*
 * Provide the composite revision operation used by this module and its client
 * applications.
 */
static uint64_t composite_revision(
    const UmiEditorCodeActionOrchestration *orchestration)
{
    uint64_t revision;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return 0U;
    revision = add_revision(
        orchestration->revision,
        umi_editor_code_action_provider_registry_revision(
            orchestration->providers));
    revision = add_revision(
        revision, umi_editor_code_action_query_revision(orchestration->query));
    revision = add_revision(
        revision, umi_editor_refactoring_plan_revision(orchestration->plan));
    revision = add_revision(
        revision,
        umi_editor_refactoring_preview_revision(orchestration->preview));
    return add_revision(
        revision,
        umi_editor_edit_transaction_revision(orchestration->transaction));
}

/*
 * Initialise editor code action orchestration from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_code_action_orchestration_create(
    UmiEditorCodeActionOrchestration **out_orchestration)
{
    UmiEditorCodeActionOrchestration *orchestration;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_orchestration = NULL;
    orchestration = (UmiEditorCodeActionOrchestration *)calloc(
        1U, sizeof(*orchestration));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_code_action_provider_registry_create(
        &orchestration->providers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_code_action_query_create(&orchestration->query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_refactoring_plan_create(&orchestration->plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_refactoring_preview_create(&orchestration->preview);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_edit_transaction_create(&orchestration->transaction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    status = umi_editor_workspace_edit_set_create(
        &orchestration->selected_edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    orchestration->revision = 1U;
    *out_orchestration = orchestration;
    return UMI_STATUS_OK;

fail:
    umi_editor_code_action_orchestration_destroy(orchestration);
    return status;
}

/*
 * Release or reset state held by editor code action orchestration so the same storage can
 * be reused safely.
 */
void umi_editor_code_action_orchestration_destroy(
    UmiEditorCodeActionOrchestration *orchestration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return;
    umi_editor_workspace_edit_set_destroy(orchestration->selected_edits);
    umi_editor_edit_transaction_destroy(orchestration->transaction);
    umi_editor_refactoring_preview_destroy(orchestration->preview);
    umi_editor_refactoring_plan_destroy(orchestration->plan);
    umi_editor_code_action_query_destroy(orchestration->query);
    umi_editor_code_action_provider_registry_destroy(orchestration->providers);
    free(orchestration);
}

/*
 * Provide the editor code action orchestration providers operation used by this module and
 * its client applications.
 */
UmiEditorCodeActionProviderRegistry *
umi_editor_code_action_orchestration_providers(
    UmiEditorCodeActionOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->providers : NULL;
}

/*
 * Provide the editor code action orchestration query operation used by this module and its
 * client applications.
 */
UmiEditorCodeActionQuery *umi_editor_code_action_orchestration_query(
    UmiEditorCodeActionOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->query : NULL;
}

/*
 * Provide the editor code action orchestration plan operation used by this module and its
 * client applications.
 */
UmiEditorRefactoringPlan *umi_editor_code_action_orchestration_plan(
    UmiEditorCodeActionOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->plan : NULL;
}

/*
 * Provide the editor code action orchestration preview operation used by this module and
 * its client applications.
 */
UmiEditorRefactoringPreview *umi_editor_code_action_orchestration_preview(
    UmiEditorCodeActionOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->preview : NULL;
}

/*
 * Provide the editor code action orchestration transaction operation used by this module
 * and its client applications.
 */
UmiEditorEditTransaction *umi_editor_code_action_orchestration_transaction(
    UmiEditorCodeActionOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->transaction : NULL;
}

/*
 * Provide the editor code action orchestration refresh preview operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_refresh_preview(
    UmiEditorCodeActionOrchestration *orchestration)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_refactoring_preview_build(orchestration->preview,
                                                  orchestration->plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        orchestration->revision = next_revision(orchestration->revision);
    }
    return status;
}

/*
 * Provide the editor code action orchestration prepare transaction operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_prepare_transaction(
    UmiEditorCodeActionOrchestration *orchestration,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_refactoring_preview_selected_edits(
        orchestration->preview, orchestration->selected_edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_edit_transaction_prepare(
        orchestration->transaction, orchestration->selected_edits, documents,
        document_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        orchestration->revision = next_revision(orchestration->revision);
    }
    return status;
}

/*
 * Provide the editor code action orchestration commit transaction operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_commit_transaction(
    UmiEditorCodeActionOrchestration *orchestration)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_edit_transaction_commit(orchestration->transaction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_refactoring_plan_mark_applied(orchestration->plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor code action orchestration cancel operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_cancel(
    UmiEditorCodeActionOrchestration *orchestration)
{
    UmiStatus transaction_status;
    UmiStatus plan_status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    transaction_status =
        umi_editor_edit_transaction_cancel(orchestration->transaction);
    plan_status = umi_editor_refactoring_plan_cancel(orchestration->plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (transaction_status != UMI_STATUS_OK) return transaction_status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (plan_status != UMI_STATUS_OK) return plan_status;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor code action orchestration snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_code_action_orchestration_snapshot(
    const UmiEditorCodeActionOrchestration *orchestration,
    UmiEditorCodeActionOrchestrationSnapshot *out_snapshot)
{
    UmiEditorRefactoringPlanSnapshot plan_snapshot;
    UmiEditorEditTransactionSnapshot transaction_snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_CODE_ACTION_ORCHESTRATION_API_VERSION;
    out_snapshot->provider_count =
        umi_editor_code_action_provider_registry_count(orchestration->providers);
    out_snapshot->action_count =
        umi_editor_code_action_query_count(orchestration->query);
    status = umi_editor_refactoring_plan_snapshot(orchestration->plan,
                                                  &plan_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_edit_transaction_snapshot(orchestration->transaction,
                                                  &transaction_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->refactoring_edit_count = plan_snapshot.edit_count;
    out_snapshot->preview_item_count =
        umi_editor_refactoring_preview_count(orchestration->preview);
    out_snapshot->plan_state = plan_snapshot.state;
    out_snapshot->transaction_state = transaction_snapshot.state;
    out_snapshot->revision = composite_revision(orchestration);
    out_snapshot->quick_fixes_enabled = 1;
    out_snapshot->refactorings_enabled = 1;
    out_snapshot->preview_enabled = 1;
    out_snapshot->atomic_transactions_enabled = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor code action orchestration revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_code_action_orchestration_revision(
    const UmiEditorCodeActionOrchestration *orchestration)
{
    return composite_revision(orchestration);
}
