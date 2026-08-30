/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_navigation_insights.c
 *
 * PURPOSE:
 *   Implement the test editor navigation insights behavior for
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
 * Umicom Framework source navigation insight coordinator tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/navigation_insights.h"

static UmiEditorNavigationResult result(UmiEditorSourceLocationKind kind)
{
    UmiEditorNavigationResult value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    (void)strcpy(value.provider_id, "test-provider");
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///source.c",
                                                  7U,
                                                  1U) == UMI_STATUS_OK);
    value.location.kind = kind;
    return value;
}

int main(void)
{
    UmiEditorNavigationInsights *insights = NULL;
    UmiEditorNavigationResultSet *definitions = NULL;
    UmiEditorNavigationResultSet *references = NULL;
    UmiEditorNavigationResult definition = result(
        UMI_EDITOR_SOURCE_LOCATION_DEFINITION);
    UmiEditorNavigationResult reference = result(
        UMI_EDITOR_SOURCE_LOCATION_REFERENCE);
    UmiEditorSourceLocation anchor;
    UmiEditorNavigationInsightsSnapshot snapshot;

    assert(umi_editor_source_location_initialize(&anchor,
                                                  "file:///main.c",
                                                  2U,
                                                  4U) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_DEFINITION,
               &definitions) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_REFERENCE,
               &references) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(definitions, &definition) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(references, &reference) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_insights_create(&insights) == UMI_STATUS_OK);
    assert(umi_editor_navigation_insights_open_peek(
               insights, &anchor, definitions) == UMI_STATUS_OK);
    assert(umi_editor_navigation_insights_snapshot(insights, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.active_surface ==
           UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION);
    assert(snapshot.peek_result_count == 1U);
    assert(umi_editor_navigation_insights_open_references(
               insights, references) == UMI_STATUS_OK);
    assert(umi_editor_navigation_insights_snapshot(insights, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.active_surface == UMI_EDITOR_NAVIGATION_INSIGHT_REFERENCES);
    assert(snapshot.reference_result_count == 1U);
    assert(umi_editor_navigation_insights_close(insights) == UMI_STATUS_OK);
    assert(umi_editor_navigation_insights_snapshot(insights, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.active_surface == UMI_EDITOR_NAVIGATION_INSIGHT_NONE);
    umi_editor_navigation_insights_destroy(insights);
    umi_editor_navigation_result_set_destroy(references);
    umi_editor_navigation_result_set_destroy(definitions);
    return 0;
}
