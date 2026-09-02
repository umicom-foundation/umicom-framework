/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/edit_transaction.h
 *
 * PURPOSE:
 *   Define atomic, revision-aware multi-document edit transactions with
 *   preflight validation and rollback owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_EDIT_TRANSACTION_H
#define UMICOM_EDITOR_EDIT_TRANSACTION_H

#include "umicom/editor/workspace_edit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_EDIT_TRANSACTION_API_VERSION 1U
#define UMI_EDITOR_EDIT_TRANSACTION_URI_CAPACITY 1024U

/**
 * List the named editor edit transaction state values accepted by this public contract.
 */
typedef enum UmiEditorEditTransactionState {
    UMI_EDITOR_EDIT_TRANSACTION_EMPTY = 1,
    UMI_EDITOR_EDIT_TRANSACTION_PREFLIGHTED = 2,
    UMI_EDITOR_EDIT_TRANSACTION_APPLYING = 3,
    UMI_EDITOR_EDIT_TRANSACTION_COMMITTED = 4,
    UMI_EDITOR_EDIT_TRANSACTION_ROLLED_BACK = 5,
    UMI_EDITOR_EDIT_TRANSACTION_CANCELLED = 6,
    UMI_EDITOR_EDIT_TRANSACTION_CONFLICT = 7
} UmiEditorEditTransactionState;

/**
 * Represent the editor edit transaction document data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorEditTransactionDocument {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[UMI_EDITOR_EDIT_TRANSACTION_URI_CAPACITY];
    UmiEditorTextBuffer *buffer;
    int writable;
    int require_matching_revision;
} UmiEditorEditTransactionDocument;

/**
 * Represent the editor edit transaction snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorEditTransactionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorEditTransactionState state;
    size_t edit_count;
    size_t document_count;
    size_t applied_edit_count;
    size_t rollback_document_count;
    uint64_t source_edit_revision;
    uint64_t revision;
    int atomic;
    int applicable;
} UmiEditorEditTransactionSnapshot;

/**
 * Represent the editor edit transaction data shared with callers of this public contract.
 */
typedef struct UmiEditorEditTransaction UmiEditorEditTransaction;

/**
 * Initialise editor edit transaction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_edit_transaction_create(
    UmiEditorEditTransaction **out_transaction);
/**
 * Release or reset state held by editor edit transaction so the same storage can be reused
 * safely.
 */
void umi_editor_edit_transaction_destroy(
    UmiEditorEditTransaction *transaction);
/**
 * Provide the editor edit transaction prepare operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_transaction_prepare(
    UmiEditorEditTransaction *transaction,
    const UmiEditorWorkspaceEditSet *edit_set,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count);
/**
 * Provide the editor edit transaction commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_transaction_commit(
    UmiEditorEditTransaction *transaction);
/**
 * Provide the editor edit transaction cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_transaction_cancel(
    UmiEditorEditTransaction *transaction);
/**
 * Provide the editor edit transaction snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_edit_transaction_snapshot(
    const UmiEditorEditTransaction *transaction,
    UmiEditorEditTransactionSnapshot *out_snapshot);
/**
 * Provide the editor edit transaction revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_edit_transaction_revision(
    const UmiEditorEditTransaction *transaction);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDIT_TRANSACTION_H */
