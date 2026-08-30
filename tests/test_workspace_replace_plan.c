/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_replace_plan.c
 *
 * PURPOSE:
 *   Implement the test workspace replace plan behavior for
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
 * File: tests/test_workspace_replace_plan.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/workspace_replace_plan.h"

int main(void)
{
    static const char source[] = "alpha alpha\n";
    UmiEditorWorkspaceSearchIndex *index = NULL;
    UmiEditorWorkspaceSearchPattern *pattern = NULL;
    UmiEditorWorkspaceSearchExclusionSet *exclusions = NULL;
    UmiEditorWorkspaceSearchQuery *query = NULL;
    UmiEditorWorkspaceReplacementPreview *preview = NULL;
    UmiEditorWorkspaceReplacePlan *plan = NULL;
    UmiEditorWorkspaceSearchDocumentInput input;
    UmiEditorWorkspaceSearchPatternRequest pattern_request;
    UmiEditorWorkspaceSearchQueryRequest query_request;
    UmiEditorWorkspaceReplacementRequest replacement_request;
    UmiEditorWorkspaceSearchPatternDiagnostic diagnostic;
    UmiEditorWorkspaceReplacePlanSnapshot snapshot;
    UmiEditorWorkspaceTextEdit edit;

    assert(umi_editor_workspace_search_index_create(NULL, &index) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_pattern_create(&pattern) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_exclusion_set_create(&exclusions) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_query_create(&query) == UMI_STATUS_OK);
    assert(umi_editor_workspace_replacement_preview_create(&preview) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_replace_plan_create(&plan) == UMI_STATUS_OK);

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
    umi_editor_workspace_search_pattern_request_init(&pattern_request,
                                                      "alpha");
    assert(umi_editor_workspace_search_pattern_compile(
               pattern, &pattern_request, &diagnostic) == UMI_STATUS_OK);
    umi_editor_workspace_search_query_request_init(&query_request);
    assert(umi_editor_workspace_search_query_execute(
               query, index, pattern, exclusions, &query_request) ==
           UMI_STATUS_OK);
    umi_editor_workspace_replacement_request_init(&replacement_request,
                                                   "omega");
    assert(umi_editor_workspace_replacement_preview_build(
               preview, index, query, &replacement_request) == UMI_STATUS_OK);
    assert(umi_editor_workspace_replace_plan_build(plan, preview, index) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_replace_plan_snapshot(plan, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.edit_count == 2U);
    assert(snapshot.document_count == 1U);
    assert(snapshot.applicable);
    assert(umi_editor_workspace_replace_plan_at(plan, 0U, &edit) ==
           UMI_STATUS_OK);
    assert(strcmp(edit.expected_text, "alpha") == 0);
    assert(strcmp(edit.replacement_text, "omega") == 0);
    assert(strcmp(edit.provider_id, "umicom.workspace-search") == 0);

    umi_editor_workspace_replace_plan_destroy(plan);
    umi_editor_workspace_replacement_preview_destroy(preview);
    umi_editor_workspace_search_query_destroy(query);
    umi_editor_workspace_search_exclusion_set_destroy(exclusions);
    umi_editor_workspace_search_pattern_destroy(pattern);
    umi_editor_workspace_search_index_destroy(index);
    return 0;
}
