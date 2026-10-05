/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_selection.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/selection.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/selection.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiSelectionContextTransferEqual(const UmiSelectionContext *a, const UmiSelectionContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->selection_id, b->selection_id) == 0 &&
        strcmp(a->selection_type, b->selection_type) == 0 &&
        strcmp(a->primary_id, b->primary_id) == 0 &&
        strcmp(a->secondary_id, b->secondary_id) == 0 &&
        a->index == b->index &&
        a->count == b->count &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiSelectionContextTransferTails(UmiSelectionContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->selection_id) + 1U;
        memset(value->selection_id + used, 0xa5, sizeof(value->selection_id) - used);
    }
    {
        size_t used = strlen(value->selection_type) + 1U;
        memset(value->selection_type + used, 0xa5, sizeof(value->selection_type) - used);
    }
    {
        size_t used = strlen(value->primary_id) + 1U;
        memset(value->primary_id + used, 0xa5, sizeof(value->primary_id) - used);
    }
    {
        size_t used = strlen(value->secondary_id) + 1U;
        memset(value->secondary_id + used, 0xa5, sizeof(value->secondary_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiSelectionContextTransferMalformed(const UmiSelectionContext *sample)
{
    (void)sample;
    {
        UmiSelectionContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.selection_id, 'x', sizeof(invalid.selection_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_selection_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_selection_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated selection_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSelectionContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.selection_type, 'x', sizeof(invalid.selection_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_selection_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_selection_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated selection_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSelectionContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.primary_id, 'x', sizeof(invalid.primary_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_selection_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_selection_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated primary_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSelectionContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.secondary_id, 'x', sizeof(invalid.secondary_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_selection_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_selection_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated secondary_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiSelectionContextTransferCases, UmiSelectionContext,
    umi_selection_context_archive_encode, umi_selection_context_archive_decode,
    UmiSelectionContextTransferEqual, UmiSelectionContextTransferTails, UmiSelectionContextTransferMalformed)

int main(void)
{
    UmiSelectionContext value;
    umi_selection_context_init(&value);
    value.selection_id[0] = 's';
    value.selection_type[0] = 's';
    value.primary_id[0] = 's';
    value.secondary_id[0] = 's';
    value.index = (uint64_t)17U;
    value.count = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_selection_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiSelectionContextTransferCases(&value) != 0) return 1;

    return 0;
}
