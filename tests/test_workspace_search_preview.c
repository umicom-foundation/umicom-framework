/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_preview.c
 *
 * PURPOSE:
 *   Implement the test workspace search preview behavior for
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
 * File: tests/test_workspace_search_preview.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/workspace_search_preview.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char source[] = "alpha 12\nALPHA 34\n";
    UmiEditorWorkspaceSearchIndex *index = NULL;
    UmiEditorWorkspaceSearchPattern *pattern = NULL;
    UmiEditorWorkspaceSearchExclusionSet *exclusions = NULL;
    UmiEditorWorkspaceSearchQuery *query = NULL;
    UmiEditorWorkspaceReplacementPreview *preview = NULL;
    UmiEditorWorkspaceSearchDocumentInput input;
    UmiEditorWorkspaceSearchPatternRequest pattern_request;
    UmiEditorWorkspaceSearchQueryRequest query_request;
    UmiEditorWorkspaceReplacementRequest replacement_request;
    UmiEditorWorkspaceSearchPatternDiagnostic diagnostic;
    UmiEditorWorkspaceReplacementPreviewSnapshot snapshot;
    UmiEditorWorkspaceReplacementPreviewItem item;

    assert(umi_editor_workspace_search_index_create(NULL, &index) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_create(&pattern) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_exclusion_set_create(&exclusions) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_query_create(&query) == UMI_STATUS_OK);
    assert(umi_editor_workspace_replacement_preview_create(&preview) ==
           UMI_STATUS_OK);
    (void)memset(&input, 0, sizeof(input));
    input.struct_size = (uint32_t)sizeof(input);
    input.api_version = UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    input.uri = "file:///main.c";
    input.relative_path = "main.c";
    input.language_id = "c";
    input.content = source;
    input.content_length = strlen(source);
    assert(umi_editor_workspace_search_index_upsert(index, &input) ==
           UMI_STATUS_OK);

    umi_editor_workspace_search_pattern_request_init(
        &pattern_request, "(alpha)\\s+(\\d+)");
    pattern_request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_REGEX;
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &pattern_request, &diagnostic) == UMI_STATUS_OK);
    umi_editor_workspace_search_query_request_init(&query_request);
    assert(umi_editor_workspace_search_query_execute(
               query, index, pattern, exclusions, &query_request) ==
           UMI_STATUS_OK);
    umi_editor_workspace_replacement_request_init(&replacement_request,
                                                   "$1=$2");
    replacement_request.preserve_case = 1;
    assert(umi_editor_workspace_replacement_preview_build(
               preview, index, query, &replacement_request) == UMI_STATUS_OK);
    assert(umi_editor_workspace_replacement_preview_snapshot(
               preview, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.item_count == 2U);
    assert(snapshot.applicable_item_count == 2U);
    assert(umi_editor_workspace_replacement_preview_at(
               preview, 0U, &item) == UMI_STATUS_OK);
    assert(strcmp(item.replacement_text, "alpha=12") == 0);
    assert(item.applicable);
    assert(umi_editor_workspace_replacement_preview_at(
               preview, 1U, &item) == UMI_STATUS_OK);
    assert(strcmp(item.replacement_text, "ALPHA=34") == 0);

    umi_editor_workspace_replacement_preview_destroy(preview);
    umi_editor_workspace_search_query_destroy(query);
    umi_editor_workspace_search_exclusion_set_destroy(exclusions);
    umi_editor_workspace_search_pattern_destroy(pattern);
    umi_editor_workspace_search_index_destroy(index);
    return 0;
}
