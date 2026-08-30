/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_navigation_results.c
 *
 * PURPOSE:
 *   Implement the test editor navigation results behavior for
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
 * Umicom Framework navigation result aggregation tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/navigation_result.h"

static UmiEditorNavigationResult result(const char *provider,
                                        const char *uri,
                                        uint64_t line,
                                        int32_t rank,
                                        int primary)
{
    UmiEditorNavigationResult value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    (void)strcpy(value.provider_id, provider);
    assert(umi_editor_source_location_initialize(&value.location,
                                                  uri,
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    value.location.kind = UMI_EDITOR_SOURCE_LOCATION_DEFINITION;
    value.rank = rank;
    value.primary = primary;
    return value;
}

int main(void)
{
    UmiEditorNavigationResultSet *results = NULL;
    UmiEditorNavigationResult low = result("native", "file:///b.c", 8U, 10, 0);
    UmiEditorNavigationResult high = result("lsp", "file:///a.c", 2U, 80, 0);
    UmiEditorNavigationResult primary = result("index", "file:///z.c", 1U, 5, 1);
    UmiEditorNavigationResult selected;

    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_DEFINITION,
               &results) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &low) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &high) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &primary) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_count(results) == 3U);
    assert(umi_editor_navigation_result_set_sort(results) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_at(results, 0U, &selected) ==
           UMI_STATUS_OK);
    assert(strcmp(selected.provider_id, "index") == 0);
    assert(umi_editor_navigation_result_set_select(results, 1U) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_selected(results, &selected) ==
           UMI_STATUS_OK);
    assert(strcmp(selected.provider_id, "lsp") == 0);

    high.rank = 90;
    (void)strcpy(high.detail, "updated without duplication");
    assert(umi_editor_navigation_result_set_upsert(results, &high) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_count(results) == 3U);
    assert(umi_editor_navigation_result_set_reset(
               results,
               UMI_EDITOR_NAVIGATION_QUERY_REFERENCE) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_count(results) == 0U);
    assert(umi_editor_navigation_result_set_query_kind(results) ==
           UMI_EDITOR_NAVIGATION_QUERY_REFERENCE);

    umi_editor_navigation_result_set_destroy(results);
    return 0;
}
