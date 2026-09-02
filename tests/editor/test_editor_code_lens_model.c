/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_code_lens_model.c
 *
 * PURPOSE:
 *   Implement the test editor code lens model behavior for
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
 * File: tests/editor/test_editor_code_lens_model.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/code_lens_model.h"

/*
 * Exercise make item and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorCodeLensItem make_item(const char *id,
                                       uint64_t line,
                                       UmiEditorCodeLensKind kind)
{
    UmiEditorCodeLensItem item = {0};
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_CODE_LENS_MODEL_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    (void)strcpy(item.title, "3 references");
    (void)strcpy(item.command_id, "editor.navigate.references");
    assert(umi_editor_source_location_initialize(
               &item.location, "file:///workspace/main.c", line, 0U) ==
           UMI_STATUS_OK);
    item.kind = kind;
    item.resolution_state = UMI_EDITOR_CODE_LENS_RESOLVED;
    item.enabled = 1;
    item.visible = 1;
    return item;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorCodeLensModel *model = NULL;
    UmiEditorCodeLensItem item;
    UmiEditorCodeLensSnapshot snapshot;

    assert(umi_editor_code_lens_model_create(&model) == UMI_STATUS_OK);
    item = make_item("late", 20U, UMI_EDITOR_CODE_LENS_TEST);
    assert(umi_editor_code_lens_model_upsert(model, &item) == UMI_STATUS_OK);
    item = make_item("early", 2U, UMI_EDITOR_CODE_LENS_REFERENCE);
    assert(umi_editor_code_lens_model_upsert(model, &item) == UMI_STATUS_OK);
    assert(umi_editor_code_lens_model_sort(model) == UMI_STATUS_OK);
    assert(umi_editor_code_lens_model_at(model, 0U, &item) == UMI_STATUS_OK);
    assert(strcmp(item.id, "early") == 0);
    assert(umi_editor_code_lens_model_set_kind_visible(
               model, UMI_EDITOR_CODE_LENS_REFERENCE, 0) == UMI_STATUS_OK);
    assert(umi_editor_code_lens_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.item_count == 2U);
    assert(snapshot.visible_item_count == 1U);
    assert(snapshot.resolved_item_count == 2U);
    assert(umi_editor_code_lens_model_visible_at(model, 0U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.id, "late") == 0);
    assert(umi_editor_code_lens_model_remove(model, "early") ==
           UMI_STATUS_OK);
    assert(umi_editor_code_lens_model_count(model) == 1U);
    umi_editor_code_lens_model_destroy(model);
    return 0;
}
