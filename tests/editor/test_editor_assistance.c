/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_assistance.c
 *
 * PURPOSE:
 *   Implement the test editor assistance behavior for
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
 * File: tests/editor/test_editor_assistance.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/editor_assistance.h"

static UmiEditorSignatureHelpItem signature(const char *id, int32_t score)
{
    UmiEditorSignatureHelpItem item;

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    (void)strcpy(item.label, "calculate(int left, int right)");
    assert(umi_editor_source_location_initialize(
               &item.location, "file:///workspace/example.c", 1U, 0U) ==
           UMI_STATUS_OK);
    item.parameter_count = 2U;
    item.score = score;
    item.visible = 1;
    return item;
}

static UmiEditorParameterInformationItem parameter(const char *signature_id,
                                                   const char *label,
                                                   size_t ordinal)
{
    UmiEditorParameterInformationItem item;

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_PARAMETER_INFORMATION_API_VERSION;
    (void)strcpy(item.signature_id, signature_id);
    (void)strcpy(item.label, label);
    item.ordinal = ordinal;
    return item;
}

int main(void)
{
    UmiEditorAssistance *assistance = NULL;
    UmiEditorSignatureHelpItem integer = signature("calculate.int", 100);
    UmiEditorSignatureHelpItem wide = signature("calculate.long", 50);
    UmiEditorParameterInformationItem integer_left =
        parameter("calculate.int", "left", 0U);
    UmiEditorParameterInformationItem integer_right =
        parameter("calculate.int", "right", 1U);
    UmiEditorParameterInformationItem wide_left =
        parameter("calculate.long", "left", 0U);
    UmiEditorParameterInformationItem wide_right =
        parameter("calculate.long", "right", 1U);
    UmiEditorParameterInformationItem actual_parameter;
    UmiEditorSignatureHelpItem actual_signature;
    UmiEditorAssistanceSnapshot snapshot;
    uint64_t revision;

    assert(umi_editor_assistance_create(&assistance) == UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_upsert(
               umi_editor_assistance_signatures(assistance), &integer) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_upsert(
               umi_editor_assistance_signatures(assistance), &wide) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_upsert(
               umi_editor_assistance_parameters(assistance), &integer_left) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_upsert(
               umi_editor_assistance_parameters(assistance), &integer_right) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_upsert(
               umi_editor_assistance_parameters(assistance), &wide_left) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_upsert(
               umi_editor_assistance_parameters(assistance), &wide_right) ==
           UMI_STATUS_OK);
    revision = umi_editor_assistance_revision(assistance);
    assert(umi_editor_assistance_activate_signature(
               assistance, "calculate.int") == UMI_STATUS_OK);
    assert(umi_editor_parameter_information_active(
               umi_editor_assistance_parameters(assistance),
               &actual_parameter) == UMI_STATUS_OK);
    assert(actual_parameter.ordinal == 0U);
    assert(umi_editor_assistance_activate_parameter(assistance, 1U) ==
           UMI_STATUS_OK);
    assert(umi_editor_assistance_select_next_signature(assistance, 1) ==
           UMI_STATUS_OK);
    assert(umi_editor_signature_help_model_active(
               umi_editor_assistance_signatures(assistance),
               &actual_signature) == UMI_STATUS_OK);
    assert(strcmp(actual_signature.id, "calculate.long") == 0);
    assert(umi_editor_assistance_set_feature_enabled(
               assistance, UMI_EDITOR_ASSISTANCE_QUICK_DOCUMENTATION, 0) ==
           UMI_STATUS_OK);
    assert(!umi_editor_assistance_feature_enabled(
        assistance, UMI_EDITOR_ASSISTANCE_QUICK_DOCUMENTATION));
    assert(umi_editor_assistance_snapshot(assistance, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.signature_count == 2U);
    assert(snapshot.parameter_count == 4U);
    assert(snapshot.has_active_signature);
    assert(snapshot.has_active_parameter);
    assert(snapshot.revision > revision);
    umi_editor_assistance_destroy(assistance);
    return 0;
}
