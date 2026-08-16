/*-----------------------------------------------------------------------------
 * Umicom Framework grouped reference result tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/reference_results.h"

static UmiEditorNavigationResult reference(const char *uri,
                                           uint64_t line,
                                           const char *preview)
{
    UmiEditorNavigationResult result = {0};
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    (void)strcpy(result.provider_id, "compiler");
    assert(umi_editor_source_location_initialize(&result.location,
                                                  uri,
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    result.location.kind = UMI_EDITOR_SOURCE_LOCATION_REFERENCE;
    (void)strcpy(result.location.preview, preview);
    return result;
}

int main(void)
{
    UmiEditorNavigationResultSet *set = NULL;
    UmiEditorReferenceResults *results = NULL;
    UmiEditorNavigationResult first = reference(
        "file:///source/main.c", 3U, "call helper");
    UmiEditorNavigationResult second = reference(
        "file:///source/main.c", 8U, "call helper again");
    UmiEditorNavigationResult third = reference(
        "file:///source/helper.c", 4U, "helper definition");
    UmiEditorReferenceGroup group;
    UmiEditorReferenceResultsSnapshot snapshot;

    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_REFERENCE,
               &set) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(set, &first) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(set, &second) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(set, &third) == UMI_STATUS_OK);
    assert(umi_editor_reference_results_create(&results) == UMI_STATUS_OK);
    assert(umi_editor_reference_results_load(results, set) == UMI_STATUS_OK);
    assert(umi_editor_reference_results_group_count(results) == 2U);
    assert(umi_editor_reference_results_visible_count(results) == 3U);
    assert(umi_editor_reference_results_group_at(results, 0U, &group) ==
           UMI_STATUS_OK);
    assert(group.result_count > 0U && group.label[0] != '\0');
    assert(umi_editor_reference_results_set_filter(results, "definition") ==
           UMI_STATUS_OK);
    assert(umi_editor_reference_results_visible_count(results) == 1U);
    assert(umi_editor_reference_results_set_filter(results, "missing") ==
           UMI_STATUS_OK);
    assert(umi_editor_reference_results_visible_count(results) == 0U);
    assert(umi_editor_reference_results_select_next(results, 1) ==
           UMI_STATUS_NOT_FOUND);
    assert(umi_editor_reference_results_set_filter(results, "") ==
           UMI_STATUS_OK);
    assert(umi_editor_reference_results_set_group_expanded(results, 0U, 0) ==
           UMI_STATUS_OK);
    assert(umi_editor_reference_results_visible_count(results) < 3U);
    assert(umi_editor_reference_results_snapshot(results, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.group_count == 2U && snapshot.result_count == 3U);
    umi_editor_reference_results_destroy(results);
    umi_editor_navigation_result_set_destroy(set);
    return 0;
}
