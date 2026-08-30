/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_hover_model.c
 *
 * PURPOSE:
 *   Implement the test editor hover model behavior for
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
 * File: tests/editor/test_editor_hover_model.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/hover_model.h"

static UmiEditorHoverSection section(const char *id,
                                     int32_t priority,
                                     int visible)
{
    UmiEditorHoverSection item;

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_HOVER_MODEL_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    (void)strcpy(item.title, "Function declaration");
    (void)strcpy(item.language_id, "c");
    (void)strcpy(item.content, "int calculate(int left, int right)");
    assert(umi_editor_source_location_initialize(
               &item.location, "file:///workspace/example.c", 4U, 7U) ==
           UMI_STATUS_OK);
    item.content_kind = UMI_EDITOR_HOVER_SOURCE_CODE;
    item.resolution_state = UMI_EDITOR_HOVER_READY;
    item.priority = priority;
    item.trusted = 1;
    item.visible = visible;
    return item;
}

int main(void)
{
    UmiEditorHoverModel *model = NULL;
    UmiEditorHoverSection first = section("hover.first", 10, 1);
    UmiEditorHoverSection second = section("hover.second", 20, 0);
    UmiEditorHoverSection actual;
    UmiEditorHoverSnapshot snapshot;

    assert(umi_editor_hover_model_create(&model) == UMI_STATUS_OK);
    assert(umi_editor_hover_model_upsert(model, &first) == UMI_STATUS_OK);
    assert(umi_editor_hover_model_upsert(model, &second) == UMI_STATUS_OK);
    assert(umi_editor_hover_model_sort(model) == UMI_STATUS_OK);
    assert(umi_editor_hover_model_at(model, 0U, &actual) == UMI_STATUS_OK);
    assert(strcmp(actual.id, "hover.second") == 0);
    assert(umi_editor_hover_model_visible_at(model, 0U, &actual) ==
           UMI_STATUS_OK);
    assert(strcmp(actual.id, "hover.first") == 0);
    assert(umi_editor_hover_model_set_active(model, "hover.second") ==
           UMI_STATUS_OK);
    assert(umi_editor_hover_model_set_pinned(model, 1) == UMI_STATUS_OK);
    assert(umi_editor_hover_model_snapshot(model, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.section_count == 2U);
    assert(snapshot.visible_section_count == 1U);
    assert(snapshot.ready_section_count == 2U);
    assert(snapshot.pinned);
    assert(strcmp(snapshot.active_section_id, "hover.second") == 0);
    second.content[0] = '\0';
    assert(umi_editor_hover_model_upsert(model, &second) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_editor_hover_model_remove(model, "hover.second") ==
           UMI_STATUS_OK);
    assert(umi_editor_hover_model_active(model, &actual) == UMI_STATUS_OK);
    assert(strcmp(actual.id, "hover.first") == 0);
    umi_editor_hover_model_destroy(model);
    return 0;
}
