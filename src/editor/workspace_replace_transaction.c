/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_replace_transaction.c
 *
 * PURPOSE:
 *   Bind a validated search-replacement plan to the established Framework edit
 *   transaction while exposing search-oriented state and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_replace_transaction.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorWorkspaceReplaceTransaction {
    UmiEditorEditTransaction *transaction;
    uint64_t source_plan_revision;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the map state operation used by this module and its client applications. */
static UmiEditorWorkspaceReplaceTransactionState map_state(
    UmiEditorEditTransactionState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_EDITOR_EDIT_TRANSACTION_PREFLIGHTED:
        case UMI_EDITOR_EDIT_TRANSACTION_APPLYING:
            return UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_PREFLIGHTED;
        case UMI_EDITOR_EDIT_TRANSACTION_COMMITTED:
            return UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_COMMITTED;
        case UMI_EDITOR_EDIT_TRANSACTION_ROLLED_BACK:
            return UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_ROLLED_BACK;
        case UMI_EDITOR_EDIT_TRANSACTION_CANCELLED:
            return UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_CANCELLED;
        case UMI_EDITOR_EDIT_TRANSACTION_CONFLICT:
            return UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_CONFLICT;
        case UMI_EDITOR_EDIT_TRANSACTION_EMPTY:
        default:
            return UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_EMPTY;
    }
}

/*
 * Initialise editor workspace replace transaction from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_workspace_replace_transaction_create(
    UmiEditorWorkspaceReplaceTransaction **out_transaction)
{
    UmiEditorWorkspaceReplaceTransaction *transaction;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_transaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_transaction = NULL;
    transaction = (UmiEditorWorkspaceReplaceTransaction *)calloc(
        1U, sizeof(*transaction));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_edit_transaction_create(&transaction->transaction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(transaction);
        return status;
    }
    transaction->revision = 1U;
    *out_transaction = transaction;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor workspace replace transaction so the same storage
 * can be reused safely.
 */
void umi_editor_workspace_replace_transaction_destroy(
    UmiEditorWorkspaceReplaceTransaction *transaction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return;
    umi_editor_edit_transaction_destroy(transaction->transaction);
    transaction->transaction = NULL;
    free(transaction);
}

/*
 * Provide the editor workspace replace transaction prepare operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replace_transaction_prepare(
    UmiEditorWorkspaceReplaceTransaction *transaction,
    const UmiEditorWorkspaceReplacePlan *plan,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count)
{
    UmiEditorWorkspaceReplacePlanSnapshot plan_snapshot;
    const UmiEditorWorkspaceEditSet *edit_set;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || plan == NULL || documents == NULL ||
        document_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_replace_plan_snapshot(plan, &plan_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || !plan_snapshot.applicable) {
        return UMI_STATUS_INVALID_STATE;
    }
    edit_set = umi_editor_workspace_replace_plan_edit_set(plan);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edit_set == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_edit_transaction_prepare(
        transaction->transaction, edit_set, documents, document_count);
    transaction->source_plan_revision = plan_snapshot.revision;
    transaction->revision = next_revision(transaction->revision);
    return status;
}

/*
 * Provide the editor workspace replace transaction commit operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replace_transaction_commit(
    UmiEditorWorkspaceReplaceTransaction *transaction)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_edit_transaction_commit(transaction->transaction);
    transaction->revision = next_revision(transaction->revision);
    return status;
}

/*
 * Provide the editor workspace replace transaction cancel operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replace_transaction_cancel(
    UmiEditorWorkspaceReplaceTransaction *transaction)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_edit_transaction_cancel(transaction->transaction);
    transaction->revision = next_revision(transaction->revision);
    return status;
}

/*
 * Provide the editor workspace replace transaction snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replace_transaction_snapshot(
    const UmiEditorWorkspaceReplaceTransaction *transaction,
    UmiEditorWorkspaceReplaceTransactionSnapshot *out_snapshot)
{
    UmiEditorEditTransactionSnapshot source;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_edit_transaction_snapshot(
        transaction->transaction, &source);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_API_VERSION;
    out_snapshot->state = map_state(source.state);
    out_snapshot->edit_count = source.edit_count;
    out_snapshot->document_count = source.document_count;
    out_snapshot->applied_edit_count = source.applied_edit_count;
    out_snapshot->rollback_document_count = source.rollback_document_count;
    out_snapshot->source_plan_revision = transaction->source_plan_revision;
    out_snapshot->source_edit_revision = source.source_edit_revision;
    out_snapshot->revision = transaction->revision;
    out_snapshot->atomic = source.atomic;
    out_snapshot->applicable = source.applicable;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor workspace replace transaction revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_workspace_replace_transaction_revision(
    const UmiEditorWorkspaceReplaceTransaction *transaction)
{
    return transaction != NULL ? transaction->revision : 0U;
}
