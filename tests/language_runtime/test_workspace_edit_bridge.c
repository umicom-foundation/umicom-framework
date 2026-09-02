/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_workspace_edit_bridge.c
 *
 * PURPOSE:
 *   Verify LSP WorkspaceEdit projects into the Editor edit-set contract, stays
 *   non-applicable until protocol coordinates are resolved, then applies with
 *   revision-safe expected text obtained from the Framework text buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/workspace_edit_bridge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageRuntimeWorkspaceEdit runtime_edit = {0};
    UmiEditorWorkspaceEditSet *edit_set = NULL;
    UmiEditorWorkspaceEditSnapshot snapshot;
    UmiEditorWorkspaceTextEdit edit;
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorTextBufferView view;
    size_t applied_count = 0U;

    (void)strcpy(runtime_edit.items[0].uri, "file:///a.c");
    runtime_edit.items[0].edit.range.start.line = 1U;
    runtime_edit.items[0].edit.range.start.character = 0U;
    runtime_edit.items[0].edit.range.end.line = 1U;
    runtime_edit.items[0].edit.range.end.character = 1U;
    (void)strcpy(runtime_edit.items[0].edit.new_text, "x");
    runtime_edit.count = 1U;

    assert(umi_language_runtime_workspace_edit_to_editor(
        &runtime_edit,
        &edit_set) == UMI_STATUS_OK);

    assert(umi_editor_workspace_edit_set_snapshot(
        edit_set,
        &snapshot) == UMI_STATUS_OK);
    assert(snapshot.edit_count == 1U);
    assert(snapshot.finalized);
    assert(!snapshot.applicable);

    assert(umi_editor_workspace_edit_set_at(
        edit_set,
        0U,
        &edit) == UMI_STATUS_OK);
    assert(edit.state == UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED);

    assert(umi_editor_text_buffer_create(32U, &buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(
        buffer,
        "z\nab\n",
        strlen("z\nab\n")) == UMI_STATUS_OK);

    assert(umi_editor_workspace_edit_set_resolve_document(
        edit_set,
        "file:///a.c",
        buffer) == UMI_STATUS_OK);

    assert(umi_editor_workspace_edit_set_at(
        edit_set,
        0U,
        &edit) == UMI_STATUS_OK);
    assert(edit.state == UMI_EDITOR_WORKSPACE_EDIT_READY);
    assert(edit.location.byte_offset == 2U);
    assert(edit.location.end_byte_offset == 3U);
    assert(strcmp(edit.expected_text, "a") == 0);

    assert(umi_editor_workspace_edit_set_snapshot(
        edit_set,
        &snapshot) == UMI_STATUS_OK);
    assert(snapshot.applicable);

    assert(umi_editor_workspace_edit_set_apply_document(
        edit_set,
        "file:///a.c",
        buffer,
        1,
        &applied_count) == UMI_STATUS_OK);
    assert(applied_count == 1U);

    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(view.byte_count == strlen("z\nxb\n"));
    assert(memcmp(view.bytes, "z\nxb\n", view.byte_count) == 0);

    umi_editor_text_buffer_destroy(buffer);
    umi_editor_workspace_edit_set_destroy(edit_set);
    return 0;
}
