/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/field.c
 *
 * PURPOSE:
 *   Implement typed selection field initialisation, setters and validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/field.h"

#include <string.h>

void umi_workbench_selection_field_init(
    UmiWorkbenchSelectionField *field,
    const char *name)
{
    if (field == NULL) return;
    memset(field, 0, sizeof(*field));
    if (name != NULL) {
        (void)umi_workbench_selection_copy_text(
            field->name, sizeof(field->name), name);
    }
}

UmiStatus umi_workbench_selection_field_set_text(
    UmiWorkbenchSelectionField *field,
    const char *value)
{
    UmiStatus status;
    if (field == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_selection_copy_text(
        field->text, sizeof(field->text), value);
    if (status == UMI_STATUS_OK) field->kind = UMI_WORKBENCH_SELECTION_VALUE_TEXT;
    return status;
}

UmiStatus umi_workbench_selection_field_set_integer(
    UmiWorkbenchSelectionField *field,
    int64_t value)
{
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    field->integer_value = value;
    field->kind = UMI_WORKBENCH_SELECTION_VALUE_INTEGER;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_field_set_unsigned(
    UmiWorkbenchSelectionField *field,
    uint64_t value)
{
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    field->unsigned_value = value;
    field->kind = UMI_WORKBENCH_SELECTION_VALUE_UNSIGNED;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_field_set_decimal(
    UmiWorkbenchSelectionField *field,
    double value)
{
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    field->decimal_value = value;
    field->kind = UMI_WORKBENCH_SELECTION_VALUE_DECIMAL;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_field_set_boolean(
    UmiWorkbenchSelectionField *field,
    bool value)
{
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    field->boolean_value = value;
    field->kind = UMI_WORKBENCH_SELECTION_VALUE_BOOLEAN;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_field_validate(
    const UmiWorkbenchSelectionField *field)
{
    if (field == NULL || field->name[0] == '\0' ||
        field->kind < UMI_WORKBENCH_SELECTION_VALUE_TEXT ||
        field->kind > UMI_WORKBENCH_SELECTION_VALUE_BOOLEAN) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_TEXT &&
        !umi_workbench_selection_text_is_valid(
            field->text, sizeof(field->text))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
