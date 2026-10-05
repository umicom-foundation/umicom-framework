/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_computed_value.c
 *
 * PURPOSE:
 *   Exercise the computed value reactive UI contract.
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
#include "umicom/ui/reactive/computed_value.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/computed_value.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveComputedValueTransferEqual(const UmiUiReactiveComputedValue *a, const UmiUiReactiveComputedValue *b)
{
    return strcmp(a->computed_id, b->computed_id) == 0 &&
        a->value.kind == b->value.kind &&
        a->value.boolean_value == b->value.boolean_value &&
        a->value.integer_value == b->value.integer_value &&
        a->value.real_value == b->value.real_value &&
        strcmp(a->value.string_value, b->value.string_value) == 0 &&
        a->revision == b->revision &&
        a->valid == b->valid;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveComputedValueTransferTails(UmiUiReactiveComputedValue *value)
{
    (void)value;
    {
        size_t used = strlen(value->computed_id) + 1U;
        memset(value->computed_id + used, 0xa5, sizeof(value->computed_id) - used);
    }
    {
        size_t used = strlen(value->value.string_value) + 1U;
        memset(value->value.string_value + used, 0xa5, sizeof(value->value.string_value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveComputedValueTransferMalformed(const UmiUiReactiveComputedValue *sample)
{
    (void)sample;
    {
        UmiUiReactiveComputedValue invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.computed_id, 'x', sizeof(invalid.computed_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_computed_value_valid(&invalid)) ||
            umi_ui_reactive_computed_value_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated computed_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiReactiveComputedValue invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.string_value, 'x', sizeof(invalid.value.string_value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_reactive_computed_value_valid(&invalid)) ||
            umi_ui_reactive_computed_value_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.string_value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveComputedValueTransferCases, UmiUiReactiveComputedValue,
    umi_ui_reactive_computed_value_archive_encode, umi_ui_reactive_computed_value_archive_decode,
    UmiUiReactiveComputedValueTransferEqual, UmiUiReactiveComputedValueTransferTails, UmiUiReactiveComputedValueTransferMalformed)

int main(void) { UmiUiReactiveComputedValue item; umi_ui_reactive_computed_value_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveComputedValue populated = item;
    (void)snprintf(populated.computed_id, sizeof(populated.computed_id), "field-0");
    populated.value.boolean_value = (int)4;
    populated.value.integer_value = (int64_t)5;
    populated.value.real_value = 6.25;
    (void)snprintf(populated.value.string_value, sizeof(populated.value.string_value), "field-5");
    populated.revision = (uint64_t)8;
    populated.valid = true;
    if (UmiUiReactiveComputedValueTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_computed_value_valid(&item) ? 0 : 1; }
