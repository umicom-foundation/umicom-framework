/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/edit_transaction.c
 *
 * PURPOSE:
 *   Implement preflighted all-or-rollback application of canonical workspace
 *   edits across multiple Framework text buffers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/edit_transaction.h"

#include <stdlib.h>
#include <string.h>

typedef struct TransactionDocumentEntry {
    UmiEditorEditTransactionDocument document;
    char *backup;
    size_t backup_size;
    uint64_t original_revision;
    size_t edit_count;
    int applied;
} TransactionDocumentEntry;

struct UmiEditorEditTransaction {
    UmiEditorWorkspaceEditSet *edits;
    TransactionDocumentEntry *documents;
    size_t document_count;
    size_t applied_edit_count;
    size_t rollback_document_count;
    uint64_t source_edit_revision;
    uint64_t revision;
    UmiEditorEditTransactionState state;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the clear documents operation used by this module and its client applications. */
static void clear_documents(UmiEditorEditTransaction *transaction)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < transaction->document_count; ++index) {
        free(transaction->documents[index].backup);
        transaction->documents[index].backup = NULL;
    }
    free(transaction->documents);
    transaction->documents = NULL;
    transaction->document_count = 0U;
    transaction->applied_edit_count = 0U;
    transaction->rollback_document_count = 0U;
}

/* Provide the find document operation used by this module and its client applications. */
static size_t find_document(const UmiEditorEditTransaction *transaction,
                            const char *uri)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < transaction->document_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(transaction->documents[index].document.uri, uri) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Provide the validate document operation used by this module and its client applications. */
static UmiStatus validate_document(
    const UmiEditorEditTransactionDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL ||
        document->struct_size != (uint32_t)sizeof(*document) ||
        document->api_version != UMI_EDITOR_EDIT_TRANSACTION_API_VERSION ||
        !terminated(document->uri, sizeof(document->uri)) ||
        document->uri[0] == '\0' || document->buffer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the copy edits operation used by this module and its client applications. */
static UmiStatus copy_edits(UmiEditorWorkspaceEditSet *target,
                            const UmiEditorWorkspaceEditSet *source,
                            size_t count)
{
    size_t index;
    UmiEditorWorkspaceTextEdit edit;
    UmiStatus status = umi_editor_workspace_edit_set_clear(target);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        status = umi_editor_workspace_edit_set_at(source, index, &edit);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_editor_workspace_edit_set_upsert(target, &edit);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_editor_workspace_edit_set_finalize(target);
}

/* Provide the capture document operation used by this module and its client applications. */
static UmiStatus capture_document(TransactionDocumentEntry *entry)
{
    UmiEditorTextBufferView view;
    UmiStatus status = umi_editor_text_buffer_view(entry->document.buffer,
                                                    &view);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (view.byte_count == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->backup = (char *)malloc(view.byte_count + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry->backup == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (view.byte_count > 0U) {
        (void)memcpy(entry->backup, view.bytes, view.byte_count);
    }
    entry->backup[view.byte_count] = '\0';
    entry->backup_size = view.byte_count;
    entry->original_revision = view.revision;
    return UMI_STATUS_OK;
}

/* Provide the preflight edit operation used by this module and its client applications. */
static UmiStatus preflight_edit(const UmiEditorWorkspaceTextEdit *edit,
                                TransactionDocumentEntry *entry)
{
    UmiEditorTextBufferView view;
    uint64_t span;
    size_t start;
    size_t length;
    UmiStatus status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!entry->document.writable) return UMI_STATUS_PERMISSION_DENIED;
    status = umi_editor_text_buffer_view(entry->document.buffer, &view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (edit->location.byte_offset > SIZE_MAX ||
        edit->location.end_byte_offset > SIZE_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    span = edit->location.end_byte_offset - edit->location.byte_offset;
    start = (size_t)edit->location.byte_offset;
    length = (size_t)span;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (start > view.byte_count || length > view.byte_count - start ||
        memcmp(view.bytes + start, edit->expected_text, length) != 0 ||
        (entry->document.require_matching_revision &&
         edit->location.document_revision != 0U &&
         edit->location.document_revision != view.revision)) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the rollback documents operation used by this module and its client
 * applications.
 */
static void rollback_documents(UmiEditorEditTransaction *transaction)
{
    size_t index;

    transaction->rollback_document_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < transaction->document_count; ++index) {
        TransactionDocumentEntry *entry = &transaction->documents[index];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (!entry->applied || entry->backup == NULL) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_text_buffer_set(entry->document.buffer, entry->backup,
                                       entry->backup_size) == UMI_STATUS_OK) {
            ++transaction->rollback_document_count;
        }
    }
}

/*
 * Initialise editor edit transaction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_edit_transaction_create(
    UmiEditorEditTransaction **out_transaction)
{
    UmiEditorEditTransaction *transaction;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_transaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_transaction = NULL;
    transaction = (UmiEditorEditTransaction *)calloc(1U, sizeof(*transaction));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_workspace_edit_set_create(&transaction->edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(transaction);
        return status;
    }
    transaction->state = UMI_EDITOR_EDIT_TRANSACTION_EMPTY;
    transaction->revision = 1U;
    *out_transaction = transaction;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor edit transaction so the same storage can be reused
 * safely.
 */
void umi_editor_edit_transaction_destroy(
    UmiEditorEditTransaction *transaction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return;
    clear_documents(transaction);
    umi_editor_workspace_edit_set_destroy(transaction->edits);
    transaction->edits = NULL;
    free(transaction);
}

/*
 * Provide the editor edit transaction prepare operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_transaction_prepare(
    UmiEditorEditTransaction *transaction,
    const UmiEditorWorkspaceEditSet *edit_set,
    const UmiEditorEditTransactionDocument *documents,
    size_t document_count)
{
    UmiEditorWorkspaceEditSnapshot edit_snapshot;
    UmiEditorWorkspaceTextEdit edit;
    size_t index;
    size_t document_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || edit_set == NULL || documents == NULL ||
        document_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_edit_set_snapshot(edit_set, &edit_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!edit_snapshot.finalized || !edit_snapshot.applicable) {
        return UMI_STATUS_INVALID_STATE;
    }
    clear_documents(transaction);
    transaction->state = UMI_EDITOR_EDIT_TRANSACTION_EMPTY;
    transaction->documents = (TransactionDocumentEntry *)calloc(
        document_count, sizeof(*transaction->documents));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction->documents == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    transaction->document_count = document_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (validate_document(&documents[index]) != UMI_STATUS_OK) {
            clear_documents(transaction);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (find_document(transaction, documents[index].uri) < index) {
            clear_documents(transaction);
            return UMI_STATUS_ALREADY_EXISTS;
        }
        transaction->documents[index].document = documents[index];
        transaction->documents[index].document.writable =
            documents[index].writable != 0;
        transaction->documents[index].document.require_matching_revision =
            documents[index].require_matching_revision != 0;
        status = capture_document(&transaction->documents[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            clear_documents(transaction);
            return status;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < edit_snapshot.edit_count; ++index) {
        status = umi_editor_workspace_edit_set_at(edit_set, index, &edit);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            clear_documents(transaction);
            return status;
        }
        document_index = find_document(transaction, edit.location.uri);
        /* Apply this branch only when its contract condition is satisfied. */
        if (document_index == SIZE_MAX) {
            clear_documents(transaction);
            transaction->state = UMI_EDITOR_EDIT_TRANSACTION_CONFLICT;
            transaction->revision = next_revision(transaction->revision);
            return UMI_STATUS_NOT_FOUND;
        }
        status = preflight_edit(&edit, &transaction->documents[document_index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            clear_documents(transaction);
            transaction->state = UMI_EDITOR_EDIT_TRANSACTION_CONFLICT;
            transaction->revision = next_revision(transaction->revision);
            return status;
        }
        ++transaction->documents[document_index].edit_count;
    }
    status = copy_edits(transaction->edits, edit_set, edit_snapshot.edit_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        clear_documents(transaction);
        return status;
    }
    transaction->source_edit_revision = edit_snapshot.revision;
    transaction->state = UMI_EDITOR_EDIT_TRANSACTION_PREFLIGHTED;
    transaction->revision = next_revision(transaction->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor edit transaction commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_transaction_commit(
    UmiEditorEditTransaction *transaction)
{
    size_t index;
    size_t applied;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (transaction->state != UMI_EDITOR_EDIT_TRANSACTION_PREFLIGHTED) {
        return UMI_STATUS_INVALID_STATE;
    }
    transaction->state = UMI_EDITOR_EDIT_TRANSACTION_APPLYING;
    transaction->applied_edit_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < transaction->document_count; ++index) {
        TransactionDocumentEntry *entry = &transaction->documents[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->edit_count == 0U) continue;
        applied = 0U;
        status = umi_editor_workspace_edit_set_apply_document(
            transaction->edits, entry->document.uri, entry->document.buffer,
            entry->document.require_matching_revision, &applied);
        transaction->applied_edit_count += applied;
        /* Apply this branch only when its contract condition is satisfied. */
        if (applied > 0U) entry->applied = 1;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            rollback_documents(transaction);
            transaction->state = UMI_EDITOR_EDIT_TRANSACTION_ROLLED_BACK;
            transaction->revision = next_revision(transaction->revision);
            return status;
        }
    }
    transaction->state = UMI_EDITOR_EDIT_TRANSACTION_COMMITTED;
    transaction->revision = next_revision(transaction->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor edit transaction cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_transaction_cancel(
    UmiEditorEditTransaction *transaction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (transaction->state == UMI_EDITOR_EDIT_TRANSACTION_COMMITTED ||
        transaction->state == UMI_EDITOR_EDIT_TRANSACTION_APPLYING) {
        return UMI_STATUS_INVALID_STATE;
    }
    clear_documents(transaction);
    (void)umi_editor_workspace_edit_set_clear(transaction->edits);
    transaction->state = UMI_EDITOR_EDIT_TRANSACTION_CANCELLED;
    transaction->revision = next_revision(transaction->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor edit transaction snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_edit_transaction_snapshot(
    const UmiEditorEditTransaction *transaction,
    UmiEditorEditTransactionSnapshot *out_snapshot)
{
    UmiEditorWorkspaceEditSnapshot edit_snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_EDIT_TRANSACTION_API_VERSION;
    out_snapshot->state = transaction->state;
    out_snapshot->document_count = transaction->document_count;
    out_snapshot->applied_edit_count = transaction->applied_edit_count;
    out_snapshot->rollback_document_count =
        transaction->rollback_document_count;
    out_snapshot->source_edit_revision = transaction->source_edit_revision;
    out_snapshot->revision = transaction->revision;
    out_snapshot->atomic = 1;
    status = umi_editor_workspace_edit_set_snapshot(transaction->edits,
                                                    &edit_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->edit_count = edit_snapshot.edit_count;
    out_snapshot->applicable =
        transaction->state == UMI_EDITOR_EDIT_TRANSACTION_PREFLIGHTED;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor edit transaction revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_edit_transaction_revision(
    const UmiEditorEditTransaction *transaction)
{
    return transaction != NULL ? transaction->revision : 0U;
}
