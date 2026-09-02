/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_linked_editing_model.c
 *
 * PURPOSE:
 *   Implement the test editor linked editing model behavior for
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
 * File: tests/editor/test_editor_linked_editing_model.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/linked_editing_model.h"

/*
 * Exercise linked range and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorLinkedRange linked_range(const char *id,
                                         uint64_t start,
                                         uint64_t revision,
                                         int primary)
{
    UmiEditorLinkedRange range = {0};

    range.struct_size = (uint32_t)sizeof(range);
    range.api_version = UMI_EDITOR_LINKED_EDITING_API_VERSION;
    (void)strcpy(range.id, id);
    (void)strcpy(range.group_id, "counter");
    (void)strcpy(range.provider_id, "native-c");
    (void)strcpy(range.original_text, "value");
    assert(umi_editor_source_location_initialize(
               &range.location, "file:///workspace/main.c", 0U, start) ==
           UMI_STATUS_OK);
    range.location.byte_offset = start;
    range.location.end_byte_offset = start + 5U;
    range.location.end_column = start + 5U;
    range.location.document_revision = revision;
    range.primary = primary;
    range.editable = 1;
    return range;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorLinkedEditingModel *model = NULL;
    UmiEditorWorkspaceEditSet *edits = NULL;
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorTextBufferView view;
    UmiEditorLinkedRange first;
    UmiEditorLinkedRange second;
    UmiEditorLinkedEditingSnapshot snapshot;
    size_t applied = 0U;

    assert(umi_editor_text_buffer_create(0U, &buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(buffer, "value + value", 13U) ==
           UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    first = linked_range("first", 0U, view.revision, 1);
    second = linked_range("second", 8U, view.revision, 0);
    assert(umi_editor_linked_editing_model_create(&model) == UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_create(&edits) == UMI_STATUS_OK);
    assert(umi_editor_linked_editing_model_upsert(model, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_linked_editing_model_upsert(model, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_linked_editing_model_finalize(model) == UMI_STATUS_OK);
    assert(umi_editor_linked_editing_model_set_active_group(model, "counter") ==
           UMI_STATUS_OK);
    assert(umi_editor_linked_editing_model_build_edit_set(
               model, "total", edits) == UMI_STATUS_OK);
    assert(umi_editor_workspace_edit_set_apply_document(
               edits, "file:///workspace/main.c", buffer, 1, &applied) ==
           UMI_STATUS_OK);
    assert(applied == 2U);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(strcmp(view.bytes, "total + total") == 0);
    assert(umi_editor_linked_editing_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.group_count == 1U);
    assert(snapshot.active_group_range_count == 2U);
    assert(snapshot.conflict_count == 0U);
    umi_editor_workspace_edit_set_destroy(edits);
    umi_editor_linked_editing_model_destroy(model);
    umi_editor_text_buffer_destroy(buffer);
    return 0;
}
