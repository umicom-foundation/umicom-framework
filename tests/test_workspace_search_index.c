/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_index.c
 *
 * PURPOSE:
 *   Implement the test workspace search index behavior for
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
 * File: tests/test_workspace_search_index.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/workspace_search_index.h"

/*
 * Exercise document and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorWorkspaceSearchDocumentInput document(
    const char *uri, const char *path, const char *content, uint64_t revision)
{
    UmiEditorWorkspaceSearchDocumentInput input;
    (void)memset(&input, 0, sizeof(input));
    input.struct_size = (uint32_t)sizeof(input);
    input.api_version = UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    input.uri = uri;
    input.relative_path = path;
    input.language_id = "c";
    input.content = content;
    input.content_length = strlen(content);
    input.document_revision = revision;
    return input;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorWorkspaceSearchIndex *index = NULL;
    UmiEditorWorkspaceSearchDocumentInput first;
    UmiEditorWorkspaceSearchDocumentInput second;
    UmiEditorWorkspaceSearchDocumentView view;
    UmiEditorWorkspaceSearchIndexSnapshot snapshot;

    assert(umi_editor_workspace_search_index_create(NULL, &index) ==
           UMI_STATUS_OK);
    first = document("file:///z.c", "src/z.c", "int z = 1;\n", 4U);
    second = document("file:///a.c", "src/a.c", "int a = 2;\n", 7U);
    assert(umi_editor_workspace_search_index_upsert(index, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_index_upsert(index, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_index_count(index) == 2U);
    assert(umi_editor_workspace_search_index_at(index, 0U, &view) ==
           UMI_STATUS_OK);
    assert(strcmp(view.relative_path, "src/a.c") == 0);
    assert(strcmp(view.file_name, "a.c") == 0);
    assert(strcmp(view.extension, "c") == 0);
    assert(view.fingerprint != 0U);

    second.content = "int a = 22;\n";
    second.content_length = strlen(second.content);
    second.document_revision = 8U;
    assert(umi_editor_workspace_search_index_upsert(index, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_index_find_uri(
               index, "file:///a.c", &view) == UMI_STATUS_OK);
    assert(view.document_revision == 8U);
    assert(strcmp(view.content, "int a = 22;\n") == 0);

    assert(umi_editor_workspace_search_index_snapshot(index, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.document_count == 2U);
    assert(snapshot.text_document_count == 2U);
    assert(snapshot.total_content_bytes ==
           strlen("int z = 1;\n") + strlen("int a = 22;\n"));

    assert(umi_editor_workspace_search_index_remove(index, "file:///z.c") ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_index_count(index) == 1U);
    umi_editor_workspace_search_index_destroy(index);
    return 0;
}
