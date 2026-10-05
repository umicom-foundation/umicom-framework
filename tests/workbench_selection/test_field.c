/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_field.c
 *
 * PURPOSE:
 *   Verify typed structured-selection fields.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection/field.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_selection/field.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchSelectionFieldTransferEqual(const UmiWorkbenchSelectionField *a, const UmiWorkbenchSelectionField *b)
{
    return strcmp(a->name, b->name) == 0 &&
        a->kind == b->kind &&
        strcmp(a->text, b->text) == 0 &&
        a->integer_value == b->integer_value &&
        a->unsigned_value == b->unsigned_value &&
        a->decimal_value == b->decimal_value &&
        a->boolean_value == b->boolean_value;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchSelectionFieldTransferTails(UmiWorkbenchSelectionField *value)
{
    (void)value;
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchSelectionFieldTransferMalformed(const UmiWorkbenchSelectionField *sample)
{
    (void)sample;
    {
        UmiWorkbenchSelectionField invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_field_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_field_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionField invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.text, 'x', sizeof(invalid.text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_field_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_field_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchSelectionFieldTransferCases, UmiWorkbenchSelectionField,
    umi_workbench_selection_field_archive_encode, umi_workbench_selection_field_archive_decode,
    UmiWorkbenchSelectionFieldTransferEqual, UmiWorkbenchSelectionFieldTransferTails, UmiWorkbenchSelectionFieldTransferMalformed)

int main(void)
{
    UmiWorkbenchSelectionField field;
    umi_workbench_selection_field_init(&field, "branch");
    assert(umi_workbench_selection_field_set_text(
        &field, "main") == UMI_STATUS_OK);
    assert(umi_workbench_selection_field_validate(
        &field) == UMI_STATUS_OK);
    if (UmiWorkbenchSelectionFieldTransferCases(&field) != 0) return 1;

    assert(field.kind == UMI_WORKBENCH_SELECTION_VALUE_TEXT);
    assert(strcmp(field.text, "main") == 0);

    umi_workbench_selection_field_init(&field, "duration-ms");
    assert(umi_workbench_selection_field_set_unsigned(
        &field, 25U) == UMI_STATUS_OK);
    assert(field.unsigned_value == 25U);
    return 0;
}
