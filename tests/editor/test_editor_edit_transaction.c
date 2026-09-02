/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_edit_transaction.c
 *
 * PURPOSE:
 *   Implement the test editor edit transaction behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_edit_transaction.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/edit_transaction.h"

/*
 * Exercise edit and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorWorkspaceTextEdit edit(const char *id, const char *uri,
                                       const char *expected,
                                       const char *replacement,
                                       uint64_t revision)
{
    UmiEditorWorkspaceTextEdit item = {0};
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    assert(umi_editor_source_location_initialize(&item.location, uri, 0U, 0U) ==
           UMI_STATUS_OK);
    item.location.end_byte_offset = strlen(expected);
    item.location.end_column = strlen(expected);
    item.location.document_revision = revision;
    (void)strcpy(item.expected_text, expected);
    (void)strcpy(item.replacement_text, replacement);
    item.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
    item.required = 1;
    return item;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorTextBuffer *first_buffer = NULL;
    UmiEditorTextBuffer *second_buffer = NULL;
    UmiEditorWorkspaceEditSet *edits = NULL;
    UmiEditorEditTransaction *transaction = NULL;
    UmiEditorEditTransactionDocument documents[2] = {{0}};
    UmiEditorTextBufferView first_view;
    UmiEditorTextBufferView second_view;
    UmiEditorWorkspaceTextEdit first_edit;
    UmiEditorWorkspaceTextEdit second_edit;
    UmiEditorEditTransactionSnapshot snapshot;

    assert(umi_editor_text_buffer_create(0U, &first_buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_create(0U, &second_buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(first_buffer, "old-a", 5U) ==
           UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(second_buffer, "old-b", 5U) ==
           UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(first_buffer, &first_view) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(second_buffer, &second_view) == UMI_STATUS_OK);
    first_edit = edit("first", "file:///first.c", "old-a", "new-a",
                      first_view.revision);
    second_edit = edit("second", "file:///second.c", "old-b", "new-b",
                       second_view.revision);
    assert(umi_editor_workspace_edit_set_create(&edits) == UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_upsert(edits, &first_edit) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_upsert(edits, &second_edit) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_finalize(edits) == UMI_STATUS_OK);
    documents[0].struct_size = (uint32_t)sizeof(documents[0]);
    documents[0].api_version = UMI_EDITOR_EDIT_TRANSACTION_API_VERSION;
    (void)strcpy(documents[0].uri, "file:///first.c");
    documents[0].buffer = first_buffer;
    documents[0].writable = 1;
    documents[0].require_matching_revision = 1;
    documents[1] = documents[0];
    (void)strcpy(documents[1].uri, "file:///second.c");
    documents[1].buffer = second_buffer;
    assert(umi_editor_edit_transaction_create(&transaction) == UMI_STATUS_OK);
    assert(umi_editor_edit_transaction_prepare(transaction, edits, documents,
                                               2U) == UMI_STATUS_OK);
    assert(umi_editor_edit_transaction_commit(transaction) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(first_buffer, &first_view) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(second_buffer, &second_view) == UMI_STATUS_OK);
    assert(strcmp(first_view.bytes, "new-a") == 0);
    assert(strcmp(second_view.bytes, "new-b") == 0);
    assert(umi_editor_edit_transaction_snapshot(transaction, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_EDIT_TRANSACTION_COMMITTED);
    assert(snapshot.applied_edit_count == 2U);
    umi_editor_edit_transaction_destroy(transaction);

    /* A change after preflight rolls back only transaction-touched buffers. */
    transaction = NULL;
    assert(umi_editor_text_buffer_set(first_buffer, "old-a", 5U) ==
           UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(second_buffer, "old-b", 5U) ==
           UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(first_buffer, &first_view) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(second_buffer, &second_view) == UMI_STATUS_OK);
    first_edit = edit("first", "file:///first.c", "old-a", "new-a",
                      first_view.revision);
    second_edit = edit("second", "file:///second.c", "old-b", "new-b",
                       second_view.revision);
    assert(umi_editor_workspace_edit_set_clear(edits) == UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_upsert(edits, &first_edit) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_upsert(edits, &second_edit) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_finalize(edits) == UMI_STATUS_OK);
    assert(umi_editor_edit_transaction_create(&transaction) == UMI_STATUS_OK);
    assert(umi_editor_edit_transaction_prepare(transaction, edits, documents,
                                               2U) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(second_buffer, "raced", 5U) ==
           UMI_STATUS_OK);
    assert(umi_editor_edit_transaction_commit(transaction) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_editor_text_buffer_view(first_buffer, &first_view) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(second_buffer, &second_view) == UMI_STATUS_OK);
    assert(strcmp(first_view.bytes, "old-a") == 0);
    assert(strcmp(second_view.bytes, "raced") == 0);
    assert(umi_editor_edit_transaction_snapshot(transaction, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_EDIT_TRANSACTION_ROLLED_BACK);
    assert(snapshot.rollback_document_count == 1U);
    umi_editor_edit_transaction_destroy(transaction);
    umi_editor_workspace_edit_set_destroy(edits);
    umi_editor_text_buffer_destroy(second_buffer);
    umi_editor_text_buffer_destroy(first_buffer);
    return 0;
}
