/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_workspace_edit.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/workspace_edit.h"

static UmiEditorWorkspaceTextEdit make_edit(const char *id,
                                            uint64_t start,
                                            uint64_t end,
                                            uint64_t revision,
                                            const char *expected,
                                            const char *replacement)
{
    UmiEditorWorkspaceTextEdit edit = {0};
    assert(umi_editor_source_location_initialize(
               &edit.location, "file:///workspace/main.c", 0U, start) ==
           UMI_STATUS_OK);
    edit.struct_size = (uint32_t)sizeof(edit);
    edit.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    (void)strcpy(edit.id, id);
    (void)strcpy(edit.provider_id, "native-c");
    (void)strcpy(edit.expected_text, expected);
    (void)strcpy(edit.replacement_text, replacement);
    edit.location.byte_offset = start;
    edit.location.end_byte_offset = end;
    edit.location.end_column = end;
    edit.location.document_revision = revision;
    edit.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
    edit.required = 1;
    return edit;
}

int main(void)
{
    UmiEditorWorkspaceEditSet *edits = NULL;
    UmiEditorWorkspaceEditSnapshot snapshot;
    UmiEditorWorkspaceTextEdit first;
    UmiEditorWorkspaceTextEdit second;
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorTextBufferView view;
    size_t applied = 0U;

    assert(umi_editor_text_buffer_create(0U, &buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(buffer, "one two three", 13U) ==
           UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_create(&edits) == UMI_STATUS_OK);
    first = make_edit("edit-one", 0U, 3U, view.revision, "one", "ONE");
    second = make_edit("edit-three", 8U, 13U, view.revision, "three", "THREE");
    assert(umi_editor_workspace_edit_set_upsert(edits, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_upsert(edits, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_finalize(edits) == UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_apply_document(
               edits, "file:///workspace/main.c", buffer, 1, &applied) ==
           UMI_STATUS_OK);
    assert(applied == 2U);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(strcmp(view.bytes, "ONE two THREE") == 0);
    assert(umi_editor_workspace_edit_set_snapshot(edits, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.applied_count == 2U);
    assert(!snapshot.applicable);

    assert(umi_editor_workspace_edit_set_clear(edits) == UMI_STATUS_OK);
    first = make_edit("overlap-one", 0U, 3U, 0U, "ONE", "one");
    second = make_edit("overlap-two", 2U, 5U, 0U, "E t", "xyz");
    assert(umi_editor_workspace_edit_set_upsert(edits, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_upsert(edits, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_finalize(edits) == UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_snapshot(edits, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.conflict_count == 2U);
    assert(!snapshot.applicable);

    umi_editor_workspace_edit_set_destroy(edits);
    umi_editor_text_buffer_destroy(buffer);
    return 0;
}
