/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_runtime.c
 *
 * PURPOSE:
 *   Verify the operational workbench owns canonical services and exposes one
 *   consistent snapshot after search and navigation operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/intelligence_workbench/runtime.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char content[] = "alpha beta alpha\n";
    UmiEditorIntelWorkbenchRuntime *runtime = NULL;
    UmiEditorWorkspaceSearchDocumentInput document;
    UmiEditorWorkspaceSearchPatternRequest pattern;
    UmiEditorWorkspaceSearchQueryRequest query;
    UmiEditorWorkspaceSearchPatternDiagnostic diagnostic;
    UmiEditorNavigationResultSet *results = NULL;
    UmiEditorNavigationResult result;
    UmiEditorSourceLocation anchor;
    UmiEditorIntelWorkbenchSnapshot snapshot;

    assert(umi_editor_intel_workbench_runtime_create(NULL, &runtime) ==
           UMI_STATUS_OK);
    assert(umi_editor_intel_workbench_runtime_search_service(runtime) != NULL);
    assert(umi_editor_intel_workbench_runtime_navigation_service(runtime) !=
           NULL);
    assert(umi_editor_intel_workbench_runtime_code_action_service(runtime) !=
           NULL);
    (void)memset(&document, 0, sizeof(document));
    document.struct_size = (uint32_t)sizeof(document);
    document.api_version = UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    document.uri = "file:///src/main.c";
    document.relative_path = "src/main.c";
    document.language_id = "c";
    document.content = content;
    document.content_length = strlen(content);
    assert(umi_editor_workspace_search_index_upsert(
               umi_editor_workspace_search_orchestration_index(
                   umi_editor_intel_workbench_runtime_search_service(runtime)),
               &document) == UMI_STATUS_OK);
    umi_editor_workspace_search_pattern_request_init(&pattern, "alpha");
    umi_editor_workspace_search_query_request_init(&query);
    assert(umi_editor_intel_workbench_runtime_search(
               runtime, &pattern, &query, &diagnostic) == UMI_STATUS_OK);

    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_DEFINITION, &results) ==
           UMI_STATUS_OK);
    (void)memset(&result, 0, sizeof(result));
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    (void)strcpy(result.provider_id, "native-c");
    (void)strcpy(result.detail, "Definition");
    assert(umi_editor_source_location_initialize(
               &result.location, "file:///src/definition.c", 1U, 1U) ==
           UMI_STATUS_OK);
    (void)strcpy(result.location.label, "definition");
    result.sequence = 1U;
    result.rank = 100;
    result.primary = 1;
    assert(umi_editor_navigation_result_set_upsert(results, &result) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_location_initialize(
               &anchor, "file:///src/main.c", 0U, 0U) == UMI_STATUS_OK);
    assert(umi_editor_intel_workbench_runtime_open_peek(
               runtime, &anchor, results) == UMI_STATUS_OK);
    assert(umi_editor_intel_workbench_runtime_snapshot(runtime, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.search_result_count == 2U);
    assert(snapshot.search_group_count == 1U);
    assert(snapshot.navigation_result_count == 1U);
    assert(snapshot.active_navigation_surface ==
           UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION);
    assert(snapshot.revision > 1U);
    umi_editor_navigation_result_set_destroy(results);
    umi_editor_intel_workbench_runtime_destroy(runtime);
    return 0;
}
