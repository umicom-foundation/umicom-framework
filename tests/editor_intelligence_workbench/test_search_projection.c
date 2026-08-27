/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_search_projection.c
 *
 * PURPOSE:
 *   Verify search results, document groups and exclusive selection are
 *   projected from the canonical workspace-search orchestration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/intelligence_workbench/search_projection.h"

static void add_document(
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    const char *uri,
    const char *path,
    const char *content)
{
    UmiEditorWorkspaceSearchDocumentInput input;

    (void)memset(&input, 0, sizeof(input));
    input.struct_size = (uint32_t)sizeof(input);
    input.api_version = UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    input.uri = uri;
    input.relative_path = path;
    input.file_name = path;
    input.language_id = "c";
    input.content = content;
    input.content_length = strlen(content);
    assert(umi_editor_workspace_search_index_upsert(
               umi_editor_workspace_search_orchestration_index(orchestration),
               &input) == UMI_STATUS_OK);
}

int main(void)
{
    UmiEditorWorkspaceSearchOrchestration *orchestration = NULL;
    UmiEditorWorkspaceSearchPatternRequest pattern;
    UmiEditorWorkspaceSearchQueryRequest query;
    UmiEditorWorkspaceSearchPatternDiagnostic diagnostic;
    UmiEditorIntelSearchProjection projection;

    assert(umi_editor_workspace_search_orchestration_create(
               NULL, &orchestration) == UMI_STATUS_OK);
    add_document(orchestration, "file:///src/a.c", "src/a.c",
                 "alpha beta alpha\n");
    add_document(orchestration, "file:///src/b.c", "src/b.c", "alpha\n");
    umi_editor_workspace_search_pattern_request_init(&pattern, "alpha");
    umi_editor_workspace_search_query_request_init(&query);
    assert(umi_editor_workspace_search_orchestration_search(
               orchestration, &pattern, &query, &diagnostic) == UMI_STATUS_OK);
    assert(umi_editor_intel_search_projection_init(&projection) ==
           UMI_STATUS_OK);
    assert(umi_editor_intel_search_projection_refresh(
               &projection, orchestration) == UMI_STATUS_OK);
    assert(projection.results.count == 3U);
    assert(projection.groups.count == 2U);
    assert(projection.has_selection);
    assert(umi_editor_intel_search_projection_select(
               &projection, orchestration, 2U) == UMI_STATUS_OK);
    assert(projection.selected_index == 2U);
    assert(umi_editor_intel_search_projection_selected(&projection) ==
           &projection.results.items[2]);
    assert(umi_editor_intel_search_projection_select_next(
               &projection, orchestration, 0) == UMI_STATUS_NOT_FOUND);
    assert(umi_editor_intel_search_projection_select_next(
               &projection, orchestration, 1) == UMI_STATUS_OK);
    assert(projection.selected_index == 0U);
    assert(umi_editor_intel_search_projection_valid(&projection));
    umi_editor_workspace_search_orchestration_destroy(orchestration);
    return 0;
}
