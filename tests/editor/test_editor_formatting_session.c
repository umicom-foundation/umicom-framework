/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_formatting_session.c
 *
 * PURPOSE:
 *   Implement the test editor formatting session behavior for
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
 * File: tests/editor/test_editor_formatting_session.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/formatting_session.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorFormattingSession *session = NULL;
    UmiEditorFormattingRequest request = {0};
    UmiEditorWorkspaceTextEdit edit = {0};
    UmiEditorFormattingSessionSnapshot snapshot;
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorTextBufferView view;
    size_t applied = 0U;

    assert(umi_editor_text_buffer_create(0U, &buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(buffer, "int  main(void){}",
                                      strlen("int  main(void){}")) ==
           UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_EDITOR_FORMATTING_SESSION_API_VERSION;
    (void)strcpy(request.id, "format-main");
    (void)strcpy(request.provider_id, "native-c");
    (void)strcpy(request.document_uri, "file:///workspace/main.c");
    (void)strcpy(request.profile_id, "native-c");
    request.mode = UMI_EDITOR_FORMATTING_DOCUMENT;
    request.end_byte_offset = 17U;
    request.document_revision = view.revision;
    request.require_matching_revision = 1;
    edit.struct_size = (uint32_t)sizeof(edit);
    edit.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    (void)strcpy(edit.id, "spacing");
    (void)strcpy(edit.provider_id, "native-c");
    assert(umi_editor_source_location_initialize(
               &edit.location, request.document_uri, 0U, 3U) == UMI_STATUS_OK);
    edit.location.byte_offset = 3U;
    edit.location.end_byte_offset = 5U;
    edit.location.end_column = 5U;
    edit.location.document_revision = view.revision;
    (void)strcpy(edit.expected_text, "  ");
    (void)strcpy(edit.replacement_text, " ");
    edit.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
    edit.required = 1;
    assert(umi_editor_formatting_session_create(&session) == UMI_STATUS_OK);
    assert(umi_editor_formatting_session_begin(session, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_formatting_session_add_edit(session, &edit) ==
           UMI_STATUS_OK);
    assert(umi_editor_formatting_session_finalize(session) == UMI_STATUS_OK);
    assert(umi_editor_formatting_session_apply(session, buffer, &applied) ==
           UMI_STATUS_OK);
    assert(applied == 1U);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(strcmp(view.bytes, "int main(void){}") == 0);
    assert(umi_editor_formatting_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_FORMATTING_APPLIED);
    assert(snapshot.applied_count == 1U);
    umi_editor_formatting_session_destroy(session);
    umi_editor_text_buffer_destroy(buffer);
    return 0;
}
