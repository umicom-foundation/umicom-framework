/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/field.c
 *
 * PURPOSE:
 *   Implement typed selection field initialisation, setters and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/field.h"

#include <string.h>

/*
 * Initialise workbench selection field from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_selection_field_init(
    UmiWorkbenchSelectionField *field,
    const char *name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field == NULL) return;
    memset(field, 0, sizeof(*field));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name != NULL) {
        (void)umi_workbench_selection_copy_text(
            field->name, sizeof(field->name), name);
    }
}

/*
 * Provide the workbench selection field set text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_text(
    UmiWorkbenchSelectionField *field,
    const char *value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_selection_copy_text(
        field->text, sizeof(field->text), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) field->kind = UMI_WORKBENCH_SELECTION_VALUE_TEXT;
    return status;
}

/*
 * Provide the workbench selection field set integer operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_integer(
    UmiWorkbenchSelectionField *field,
    int64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    field->integer_value = value;
    field->kind = UMI_WORKBENCH_SELECTION_VALUE_INTEGER;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection field set unsigned operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_unsigned(
    UmiWorkbenchSelectionField *field,
    uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    field->unsigned_value = value;
    field->kind = UMI_WORKBENCH_SELECTION_VALUE_UNSIGNED;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection field set decimal operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_decimal(
    UmiWorkbenchSelectionField *field,
    double value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    field->decimal_value = value;
    field->kind = UMI_WORKBENCH_SELECTION_VALUE_DECIMAL;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection field set boolean operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_field_set_boolean(
    UmiWorkbenchSelectionField *field,
    bool value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    field->boolean_value = value;
    field->kind = UMI_WORKBENCH_SELECTION_VALUE_BOOLEAN;
    return UMI_STATUS_OK;
}

/*
 * Check that workbench selection field satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_field_validate(
    const UmiWorkbenchSelectionField *field)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field == NULL || field->name[0] == '\0' ||
        field->kind < UMI_WORKBENCH_SELECTION_VALUE_TEXT ||
        field->kind > UMI_WORKBENCH_SELECTION_VALUE_BOOLEAN) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (field->kind == UMI_WORKBENCH_SELECTION_VALUE_TEXT &&
        !umi_workbench_selection_text_is_valid(
            field->text, sizeof(field->text))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
