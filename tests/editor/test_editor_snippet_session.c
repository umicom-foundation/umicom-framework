/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_snippet_session.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/snippet_session.h"

int main(void)
{
    UmiEditorSnippetSession *session = NULL;
    UmiEditorSnippetTemplate snippet = {0};
    UmiEditorSnippetPlaceholder placeholder;
    UmiEditorSnippetSessionSnapshot snapshot;
    char expanded[256];

    snippet.struct_size = (uint32_t)sizeof(snippet);
    snippet.api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    (void)strcpy(snippet.id, "c-for-loop");
    (void)strcpy(snippet.language_id, "c");
    (void)strcpy(snippet.name, "C for loop");
    (void)strcpy(snippet.body,
                 "for (${1:size_t i = 0}; ${2:i < count}; ${1}; ++i) {\n"
                 "    ${3|work(),continue|};\n}$0");
    assert(umi_editor_snippet_session_create(&session) == UMI_STATUS_OK);
    assert(umi_editor_snippet_session_start(session, &snippet, 100U) ==
           UMI_STATUS_OK);
    assert(umi_editor_snippet_session_expanded_text(
               session, expanded, sizeof(expanded)) == UMI_STATUS_OK);
    assert(strstr(expanded, "size_t i = 0") != NULL);
    assert(strstr(expanded, "work()") != NULL);
    assert(umi_editor_snippet_session_active(session, &placeholder) ==
           UMI_STATUS_OK);
    assert(placeholder.ordinal == 1U);
    assert(placeholder.start_byte_offset >= 100U);
    assert(umi_editor_snippet_session_next(session) == UMI_STATUS_OK);
    assert(umi_editor_snippet_session_active(session, &placeholder) ==
           UMI_STATUS_OK);
    assert(placeholder.ordinal == 2U);
    assert(umi_editor_snippet_session_select(session, 3U) == UMI_STATUS_OK);
    assert(umi_editor_snippet_session_next(session) == UMI_STATUS_OK);
    assert(umi_editor_snippet_session_active(session, &placeholder) ==
           UMI_STATUS_OK);
    assert(placeholder.ordinal == 0U);
    assert(umi_editor_snippet_session_next(session) == UMI_STATUS_OK);
    assert(umi_editor_snippet_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_SNIPPET_COMPLETED);
    assert(snapshot.placeholder_count == 5U);
    assert(snapshot.traversal_stop_count == 4U);
    umi_editor_snippet_session_destroy(session);
    return 0;
}
