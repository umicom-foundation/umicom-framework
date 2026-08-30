/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_orchestration.c
 *
 * PURPOSE:
 *   Implement the test workspace search orchestration behavior for
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
 * File: tests/test_workspace_search_orchestration.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/workspace_search_orchestration.h"

int main(void)
{
    static const char source[] = "one alpha two alpha\n";
    UmiEditorWorkspaceSearchOrchestration *orchestration = NULL;
    UmiEditorWorkspaceSearchDocumentInput input;
    UmiEditorWorkspaceSearchPatternRequest pattern_request;
    UmiEditorWorkspaceSearchQueryRequest query_request;
    UmiEditorWorkspaceReplacementRequest replacement_request;
    UmiEditorWorkspaceSearchPatternDiagnostic diagnostic;
    UmiEditorWorkspaceSearchOrchestrationSnapshot snapshot;

    assert(umi_editor_workspace_search_orchestration_create(
               NULL, &orchestration) == UMI_STATUS_OK);
    (void)memset(&input, 0, sizeof(input));
    input.struct_size = (uint32_t)sizeof(input);
    input.api_version = UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    input.uri = "file:///main.c";
    input.relative_path = "main.c";
    input.language_id = "c";
    input.content = source;
    input.content_length = strlen(source);
    assert(umi_editor_workspace_search_index_upsert(
               umi_editor_workspace_search_orchestration_index(orchestration),
               &input) == UMI_STATUS_OK);

    umi_editor_workspace_search_pattern_request_init(&pattern_request,
                                                      "alpha");
    umi_editor_workspace_search_query_request_init(&query_request);
    assert(umi_editor_workspace_search_orchestration_search(
               orchestration, &pattern_request, &query_request, &diagnostic) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_orchestration_snapshot(
               orchestration, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state ==
           UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_SEARCHED);
    assert(snapshot.result_count == 2U);

    umi_editor_workspace_replacement_request_init(&replacement_request,
                                                   "omega");
    assert(umi_editor_workspace_search_orchestration_preview(
               orchestration, &replacement_request) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_orchestration_plan(orchestration) ==
           UMI_STATUS_OK);
    assert(umi_editor_workspace_search_orchestration_snapshot(
               orchestration, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state ==
           UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_PLANNED);
    assert(snapshot.preview_item_count == 2U);
    assert(snapshot.planned_edit_count == 2U);
    assert(snapshot.plan_applicable);

    assert(umi_editor_workspace_search_orchestration_clear_results(
               orchestration) == UMI_STATUS_OK);
    assert(umi_editor_workspace_search_orchestration_snapshot(
               orchestration, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state ==
           UMI_EDITOR_WORKSPACE_SEARCH_ORCHESTRATION_EMPTY);
    assert(snapshot.result_count == 0U);

    umi_editor_workspace_search_orchestration_destroy(orchestration);
    return 0;
}
