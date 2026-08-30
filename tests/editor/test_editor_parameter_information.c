/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_parameter_information.c
 *
 * PURPOSE:
 *   Implement the test editor parameter information behavior for
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
 * File: tests/editor/test_editor_parameter_information.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/parameter_information.h"

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
    (void)strcpy(item.type_name, "int");
    (void)strcpy(item.documentation, "A signed integer operand.");
    item.ordinal = ordinal;
    item.label_start = ordinal * 4U;
    item.label_end = item.label_start + strlen(label);
    item.required = 1;
    return item;
}

int main(void)
{
    UmiEditorParameterInformation *information = NULL;
    UmiEditorParameterInformationItem left =
        parameter("calculate.int", "left", 0U);
    UmiEditorParameterInformationItem right =
        parameter("calculate.int", "right", 1U);
    UmiEditorParameterInformationItem value =
        parameter("calculate.long", "value", 0U);
    UmiEditorParameterInformationItem actual;
    UmiEditorParameterInformationSnapshot snapshot;

    value.deprecated = 1;
    assert(umi_editor_parameter_information_create(&information) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_upsert(information, &right) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_upsert(information, &left) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_upsert(information, &value) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_sort(information) == UMI_STATUS_OK);
    assert(umi_editor_parameter_information_signature_count(
               information, "calculate.int") == 2U);
    assert(umi_editor_parameter_information_for_signature_at(
               information, "calculate.int", 1U, &actual) == UMI_STATUS_OK);
    assert(actual.ordinal == 1U);
    assert(umi_editor_parameter_information_set_active(
               information, "calculate.int", 1U) == UMI_STATUS_OK);
    assert(umi_editor_parameter_information_active(information, &actual) ==
           UMI_STATUS_OK);
    assert(strcmp(actual.label, "right") == 0);
    assert(umi_editor_parameter_information_snapshot(information, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.parameter_count == 3U);
    assert(snapshot.signature_count == 2U);
    assert(snapshot.required_parameter_count == 3U);
    assert(snapshot.deprecated_parameter_count == 1U);
    assert(snapshot.has_active_parameter);
    assert(umi_editor_parameter_information_clear_active(information) ==
           UMI_STATUS_OK);
    assert(umi_editor_parameter_information_active(information, &actual) ==
           UMI_STATUS_NOT_FOUND);
    assert(umi_editor_parameter_information_set_active(
               information, "calculate.int", 1U) == UMI_STATUS_OK);
    assert(umi_editor_parameter_information_remove_signature(
               information, "calculate.int") == UMI_STATUS_OK);
    assert(umi_editor_parameter_information_active(information, &actual) ==
           UMI_STATUS_NOT_FOUND);
    umi_editor_parameter_information_destroy(information);
    return 0;
}
