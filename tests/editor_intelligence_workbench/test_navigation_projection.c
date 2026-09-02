/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_navigation_projection.c
 *
 * PURPOSE:
 *   Verify peek and reference results project through the canonical navigation
 *   insights coordinator without frontend-owned navigation state.
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

#include "umicom/editor/intelligence_workbench/navigation_projection.h"

/*
 * Exercise result and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorNavigationResult result(
    const char *uri, const char *label, uint64_t sequence, int primary)
{
    UmiEditorNavigationResult item;

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    (void)strcpy(item.provider_id, "native-c");
    (void)strcpy(item.detail, "Definition target");
    assert(umi_editor_source_location_initialize(
               &item.location, uri, sequence, 1U) == UMI_STATUS_OK);
    (void)strcpy(item.location.label, label);
    item.sequence = sequence;
    item.rank = primary ? 100 : 50;
    item.primary = primary;
    return item;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorNavigationInsights *insights = NULL;
    UmiEditorNavigationResultSet *results = NULL;
    UmiEditorSourceLocation anchor;
    UmiEditorNavigationResult first =
        result("file:///src/a.c", "alpha", 1U, 1);
    UmiEditorNavigationResult second =
        result("file:///src/b.c", "beta", 2U, 0);
    UmiEditorIntelNavigationProjection projection;

    assert(umi_editor_navigation_insights_create(&insights) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_DEFINITION, &results) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_sort(results) == UMI_STATUS_OK);
    assert(umi_editor_source_location_initialize(
               &anchor, "file:///src/use.c", 4U, 2U) == UMI_STATUS_OK);
    assert(umi_editor_navigation_insights_open_peek(
               insights, &anchor, results) == UMI_STATUS_OK);
    assert(umi_editor_intel_navigation_projection_init(&projection) ==
           UMI_STATUS_OK);
    assert(umi_editor_intel_navigation_projection_refresh(
               &projection, insights) == UMI_STATUS_OK);
    assert(projection.source_snapshot.active_surface ==
           UMI_EDITOR_NAVIGATION_INSIGHT_PEEK_DEFINITION);
    assert(umi_editor_intel_navigation_projection_count(&projection) == 2U);
    assert((umi_editor_intel_navigation_projection_at(&projection, 0U)->flags &
            UMI_EDITOR_INTEL_PROJECTION_PRIMARY) != 0U);
    assert(umi_editor_navigation_result_set_reset(
               results, UMI_EDITOR_NAVIGATION_QUERY_REFERENCE) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_insights_open_references(insights, results) ==
           UMI_STATUS_OK);
    assert(umi_editor_intel_navigation_projection_refresh(
               &projection, insights) == UMI_STATUS_OK);
    assert(projection.references.count == 2U);
    assert(umi_editor_intel_navigation_projection_valid(&projection));
    umi_editor_navigation_result_set_destroy(results);
    umi_editor_navigation_insights_destroy(insights);
    return 0;
}
