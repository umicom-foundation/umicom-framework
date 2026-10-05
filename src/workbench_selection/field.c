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
#include "../base/value_archive_internal.h"

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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (field == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(field->name, '\0', sizeof(field->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(field->text, '\0', sizeof(field->text)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchSelectionFieldArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb4fe608835d73664);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionField *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionField *)0)->text)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchSelectionFieldArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchSelectionField *)0)->name) - 1U +
        8U +
        8U + sizeof(((UmiWorkbenchSelectionField *)0)->text) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchSelectionFieldArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchSelectionField *value)
{
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteText(writer, value->text, sizeof(value->text));
    UmiArchiveWriteSigned(writer, (int64_t)value->integer_value);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->unsigned_value);
    UmiArchiveWriteDouble(writer, value->decimal_value);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->boolean_value);
}
static void UmiWorkbenchSelectionFieldArchiveRead(UmiArchiveReader *reader, UmiWorkbenchSelectionField *value)
{
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->kind = (UmiWorkbenchSelectionValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->text, sizeof(value->text));
    value->integer_value = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->unsigned_value = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->decimal_value = UmiArchiveReadDouble(reader);
    value->boolean_value = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiWorkbenchSelectionFieldArchiveValidate(const UmiWorkbenchSelectionField *value)
{
    return umi_workbench_selection_field_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_selection_field_archive_encode, umi_workbench_selection_field_archive_decode,
    UmiWorkbenchSelectionField, UmiWorkbenchSelectionFieldArchiveSchema, UmiWorkbenchSelectionFieldArchiveBound, UmiWorkbenchSelectionFieldArchiveWrite, UmiWorkbenchSelectionFieldArchiveRead, UmiWorkbenchSelectionFieldArchiveValidate)
