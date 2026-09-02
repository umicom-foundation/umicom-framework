/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_signature_help_model.c
 *
 * PURPOSE:
 *   Implement the test editor signature help model behavior for
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
 * File: tests/editor/test_editor_signature_help_model.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/signature_help_model.h"

/*
 * Exercise signature and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorSignatureHelpItem signature(const char *id, int32_t score)
{
    UmiEditorSignatureHelpItem item;

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    (void)strcpy(item.label, "calculate(int left, int right)");
    (void)strcpy(item.documentation, "Calculate a result from two operands.");
    (void)strcpy(item.return_type, "int");
    assert(umi_editor_source_location_initialize(
               &item.location, "file:///workspace/example.c", 2U, 0U) ==
           UMI_STATUS_OK);
    item.parameter_count = 2U;
    item.score = score;
    item.visible = 1;
    return item;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorSignatureHelpModel *model = NULL;
    UmiEditorSignatureHelpItem integer = signature("calculate.int", 100);
    UmiEditorSignatureHelpItem wide = signature("calculate.long", 50);
    UmiEditorSignatureHelpItem actual;
    UmiEditorSignatureHelpContext context;
    UmiEditorSignatureHelpSnapshot snapshot;

    (void)memset(&context, 0, sizeof(context));
    context.struct_size = (uint32_t)sizeof(context);
    context.api_version = UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION;
    context.trigger_kind = UMI_EDITOR_SIGNATURE_HELP_TRIGGER_CHARACTER;
    context.trigger_character = (uint32_t)'(';
    context.document_revision = 9U;
    assert(umi_editor_signature_help_model_create(&model) == UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_upsert(model, &wide) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_upsert(model, &integer) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_sort(model) == UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_at(model, 0U, &actual) ==
           UMI_STATUS_OK);
    assert(strcmp(actual.id, "calculate.int") == 0);
    assert(umi_editor_signature_help_model_set_context(model, &context) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_set_active(
               model, "calculate.int") == UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_set_presented(model, 1) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_select_next(model, 1) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_active(model, &actual) ==
           UMI_STATUS_OK);
    assert(strcmp(actual.id, "calculate.long") == 0);
    assert(umi_editor_signature_help_model_select_previous(model, 1) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.signature_count == 2U);
    assert(snapshot.visible_signature_count == 2U);
    assert(snapshot.has_active_signature);
    assert(snapshot.presented);
    assert(snapshot.context.trigger_character == (uint32_t)'(');
    context.trigger_character = 0U;
    assert(umi_editor_signature_help_model_set_context(model, &context) ==
           UMI_STATUS_INVALID_ARGUMENT);
    umi_editor_signature_help_model_destroy(model);
    return 0;
}
