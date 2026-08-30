/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_peek_definition.c
 *
 * PURPOSE:
 *   Implement the test editor peek definition behavior for
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
 * Umicom Framework peek definition model tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/peek_definition.h"

static UmiEditorNavigationResult definition(const char *uri, uint64_t line)
{
    UmiEditorNavigationResult result = {0};
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    (void)strcpy(result.provider_id, "native-c-index");
    assert(umi_editor_source_location_initialize(&result.location,
                                                  uri,
                                                  line,
                                                  2U) == UMI_STATUS_OK);
    result.location.kind = UMI_EDITOR_SOURCE_LOCATION_DEFINITION;
    return result;
}

int main(void)
{
    UmiEditorPeekDefinitionModel *model = NULL;
    UmiEditorNavigationResultSet *results = NULL;
    UmiEditorNavigationResult first = definition("file:///a.c", 4U);
    UmiEditorNavigationResult second = definition("file:///b.c", 9U);
    UmiEditorNavigationResult active;
    UmiEditorSourceLocation anchor;
    UmiEditorPeekDefinitionSnapshot snapshot;

    assert(umi_editor_source_location_initialize(&anchor,
                                                  "file:///main.c",
                                                  12U,
                                                  6U) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_DEFINITION,
               &results) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_upsert(results, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_peek_definition_model_create(&model) == UMI_STATUS_OK);
    assert(umi_editor_peek_definition_model_load(model, &anchor, results) ==
           UMI_STATUS_OK);
    assert(umi_editor_peek_definition_model_count(model) == 2U);
    assert(umi_editor_peek_definition_model_select_next(model, 1) ==
           UMI_STATUS_OK);
    assert(umi_editor_peek_definition_model_active(model, &active) ==
           UMI_STATUS_OK);
    assert(strcmp(active.location.uri, "file:///b.c") == 0);
    assert(umi_editor_peek_definition_model_set_pinned(model, 1) ==
           UMI_STATUS_OK);
    assert(umi_editor_peek_definition_model_hide(model) == UMI_STATUS_OK);
    assert(umi_editor_peek_definition_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.result_count == 2U && snapshot.pinned);
    assert(!snapshot.visible && snapshot.has_anchor);
    assert(snapshot.can_select_next && snapshot.can_select_previous);
    assert(umi_editor_peek_definition_model_show(model) == UMI_STATUS_OK);
    umi_editor_peek_definition_model_destroy(model);
    umi_editor_navigation_result_set_destroy(results);
    return 0;
}
