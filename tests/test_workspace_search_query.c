/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_query.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/workspace_search_query.h"

static void add_document(UmiEditorWorkspaceSearchIndex *index,
                         const char *uri,
                         const char *path,
                         const char *content,
                         int generated)
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
    input.generated = generated;
    assert(umi_editor_workspace_search_index_upsert(index, &input) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiEditorWorkspaceSearchIndex *index = NULL;
    UmiEditorWorkspaceSearchPattern *pattern = NULL;
    UmiEditorWorkspaceSearchExclusionSet *exclusions = NULL;
    UmiEditorWorkspaceSearchQuery *query = NULL;
    UmiEditorWorkspaceSearchPatternRequest pattern_request;
    UmiEditorWorkspaceSearchQueryRequest query_request;
    UmiEditorWorkspaceSearchPatternDiagnostic diagnostic;
    UmiEditorWorkspaceSearchQuerySnapshot snapshot;
    UmiEditorWorkspaceSearchMatch match;

    assert(umi_editor_workspace_search_index_create(NULL, &index) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_create(&pattern) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_exclusion_set_create(&exclusions) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_query_create(&query) == UMI_STATUS_OK);

    add_document(index, "file:///src/main.c", "src/main.c",
                 "alpha beta alpha\n", 0);
    add_document(index, "file:///build/generated.c", "build/generated.c",
                 "alpha\n", 1);
    umi_editor_workspace_search_pattern_request_init(&pattern_request,
                                                      "alpha");
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &pattern_request, &diagnostic) == UMI_STATUS_OK);
    umi_editor_workspace_search_query_request_init(&query_request);
    assert(umi_editor_workspace_search_query_execute(
               query, index, pattern, exclusions, &query_request) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_query_snapshot(query, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.result_count == 2U);
    assert(snapshot.document_count == 1U);
    assert(snapshot.excluded_document_count == 1U);
    assert(snapshot.complete);
    assert(umi_editor_workspace_search_query_at(query, 1U, &match) ==
           UMI_STATUS_OK);
    assert(match.location.byte_offset == 11U);
    assert(match.selected);
    assert(umi_editor_workspace_search_query_select(
               query, match.id, 0) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_query_snapshot(query, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.selected_result_count == 1U);

    umi_editor_workspace_search_query_destroy(query);
    umi_editor_workspace_search_exclusion_set_destroy(exclusions);
    umi_editor_workspace_search_pattern_destroy(pattern);
    umi_editor_workspace_search_index_destroy(index);
    return 0;
}
