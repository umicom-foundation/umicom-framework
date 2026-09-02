/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_replace_transaction.h
 *
 * PURPOSE:
 *   Provide the workspace-search-specific transaction lifecycle over the
 *   Framework's existing atomic, revision-aware multi-document edit engine.
 *
 * TRANSACTION GUARANTEE:
 *   prepare() performs preflight against every supplied working document.
 *   commit() delegates atomic application and rollback to edit_transaction.
 *   No application or GUI code may bypass this transaction for Replace All.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_REPLACE_TRANSACTION_H
#define UMICOM_EDITOR_WORKSPACE_REPLACE_TRANSACTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/edit_transaction.h"
#include "umicom/editor/workspace_replace_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_API_VERSION 1U

/**
 * List the named editor workspace replace transaction state values accepted by this public
 * contract.
 */
typedef enum UmiEditorWorkspaceReplaceTransactionState {
    UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_EMPTY = 1,
    UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_PREFLIGHTED = 2,
    UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_COMMITTED = 3,
    UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_ROLLED_BACK = 4,
    UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_CANCELLED = 5,
    UMI_EDITOR_WORKSPACE_REPLACE_TRANSACTION_CONFLICT = 6
} UmiEditorWorkspaceReplaceTransactionState;

/**
 * Represent the editor workspace replace transaction snapshot data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorWorkspaceReplaceTransactionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorWorkspaceReplaceTransactionState state;
    size_t edit_count;
    size_t document_count;
    size_t applied_edit_count;
    size_t rollback_document_count;
    uint64_t source_plan_revision;
    uint64_t source_edit_revision;
    uint64_t revision;
    int atomic;
    int applicable;
} UmiEditorWorkspaceReplaceTransactionSnapshot;

/**
 * Represent the editor workspace replace transaction data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorWorkspaceReplaceTransaction
    UmiEditorWorkspaceReplaceTransaction;

/**
 * Initialise editor workspace replace transaction from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_workspace_replace_transaction_create(
    UmiEditorWorkspaceReplaceTransaction **out_transaction);
/**
 * Release or reset state held by editor workspace replace transaction so the same storage
 * can be reused safely.
 */
void umi_editor_workspace_replace_transaction_destroy(
    UmiEditorWorkspaceReplaceTransaction *transaction);

/**
 * Provide the editor workspace replace transaction prepare operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replace_transaction_prepare(
    UmiEditorWorkspaceReplaceTransaction *transaction,
    const UmiEditorWorkspaceReplacePlan *plan,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count);
/**
 * Provide the editor workspace replace transaction commit operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replace_transaction_commit(
    UmiEditorWorkspaceReplaceTransaction *transaction);
/**
 * Provide the editor workspace replace transaction cancel operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replace_transaction_cancel(
    UmiEditorWorkspaceReplaceTransaction *transaction);
/**
 * Provide the editor workspace replace transaction snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_workspace_replace_transaction_snapshot(
    const UmiEditorWorkspaceReplaceTransaction *transaction,
    UmiEditorWorkspaceReplaceTransactionSnapshot *out_snapshot);

/**
 * Provide the editor workspace replace transaction revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_workspace_replace_transaction_revision(
    const UmiEditorWorkspaceReplaceTransaction *transaction);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_REPLACE_TRANSACTION_H */
