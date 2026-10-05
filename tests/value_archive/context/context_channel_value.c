/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_value.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/value.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/value.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextValueTransferEqual(const UmiContextValue *a, const UmiContextValue *b)
{
    return a->structure_size == b->structure_size &&
        a->kind == b->kind &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->text, b->text) == 0 &&
        a->integer_value == b->integer_value &&
        a->unsigned_value == b->unsigned_value &&
        a->decimal_value == b->decimal_value &&
        a->boolean_value == b->boolean_value;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextValueTransferTails(UmiContextValue *value)
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
static int UmiContextValueTransferMalformed(const UmiContextValue *sample)
{
    (void)sample;
    {
        UmiContextValue invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_value_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_value_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextValue invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.text, 'x', sizeof(invalid.text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_value_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_value_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextValueTransferCases, UmiContextValue,
    umi_context_value_archive_encode, umi_context_value_archive_decode,
    UmiContextValueTransferEqual, UmiContextValueTransferTails, UmiContextValueTransferMalformed)

int main(void)
{
    UmiContextValue value;
    umi_context_value_init(&value, "saved");
    value.name[0] = 's';
    value.text[0] = 's';
    value.integer_value = (int64_t)17U;
    value.unsigned_value = (uint64_t)17U;
    value.decimal_value = 2.5;
    value.boolean_value = true;
    if (umi_context_value_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextValueTransferCases(&value) != 0) return 1;

    return 0;
}
