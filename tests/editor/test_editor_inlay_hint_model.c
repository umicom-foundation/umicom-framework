/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_inlay_hint_model.c
 *
 * PURPOSE:
 *   Implement the test editor inlay hint model behavior for
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
 * File: tests/editor/test_editor_inlay_hint_model.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/inlay_hint_model.h"

/*
 * Exercise make hint and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorInlayHintItem make_hint(const char *id,
                                        uint64_t column,
                                        UmiEditorInlayHintKind kind)
{
    UmiEditorInlayHintItem item = {0};
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_INLAY_HINT_MODEL_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    (void)strcpy(item.label, ": size_t");
    (void)strcpy(item.tooltip, "Inferred type");
    assert(umi_editor_source_location_initialize(
               &item.location, "file:///workspace/main.c", 3U, column) ==
           UMI_STATUS_OK);
    item.kind = kind;
    item.padding_before = 1;
    item.resolved = 1;
    item.visible = 1;
    return item;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorInlayHintModel *model = NULL;
    UmiEditorInlayHintItem item;
    UmiEditorInlayHintSnapshot snapshot;

    assert(umi_editor_inlay_hint_model_create(&model) == UMI_STATUS_OK);
    item = make_hint("type", 12U, UMI_EDITOR_INLAY_HINT_TYPE);
    assert(umi_editor_inlay_hint_model_upsert(model, &item) == UMI_STATUS_OK);
    item = make_hint("parameter", 4U, UMI_EDITOR_INLAY_HINT_PARAMETER);
    assert(umi_editor_inlay_hint_model_upsert(model, &item) == UMI_STATUS_OK);
    assert(umi_editor_inlay_hint_model_sort(model) == UMI_STATUS_OK);
    assert(umi_editor_inlay_hint_model_at(model, 0U, &item) == UMI_STATUS_OK);
    assert(strcmp(item.id, "parameter") == 0);
    assert(umi_editor_inlay_hint_model_set_kind_visible(
               model, UMI_EDITOR_INLAY_HINT_PARAMETER, 0) == UMI_STATUS_OK);
    assert(umi_editor_inlay_hint_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.item_count == 2U);
    assert(snapshot.visible_item_count == 1U);
    assert(snapshot.resolved_item_count == 2U);
    assert(umi_editor_inlay_hint_model_visible_at(model, 0U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.id, "type") == 0);
    umi_editor_inlay_hint_model_destroy(model);
    return 0;
}
