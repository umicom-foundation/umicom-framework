/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_source_location.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/source_location.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/source_location.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiSourceLocationContextTransferEqual(const UmiSourceLocationContext *a, const UmiSourceLocationContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->workspace_id, b->workspace_id) == 0 &&
        strcmp(a->file_path, b->file_path) == 0 &&
        strcmp(a->symbol, b->symbol) == 0 &&
        a->line == b->line &&
        a->column == b->column &&
        a->selection_length == b->selection_length &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiSourceLocationContextTransferTails(UmiSourceLocationContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
    {
        size_t used = strlen(value->file_path) + 1U;
        memset(value->file_path + used, 0xa5, sizeof(value->file_path) - used);
    }
    {
        size_t used = strlen(value->symbol) + 1U;
        memset(value->symbol + used, 0xa5, sizeof(value->symbol) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiSourceLocationContextTransferMalformed(const UmiSourceLocationContext *sample)
{
    (void)sample;
    {
        UmiSourceLocationContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_source_location_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_source_location_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSourceLocationContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.file_path, 'x', sizeof(invalid.file_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_source_location_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_source_location_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated file_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSourceLocationContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.symbol, 'x', sizeof(invalid.symbol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_source_location_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_source_location_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated symbol was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiSourceLocationContextTransferCases, UmiSourceLocationContext,
    umi_source_location_context_archive_encode, umi_source_location_context_archive_decode,
    UmiSourceLocationContextTransferEqual, UmiSourceLocationContextTransferTails, UmiSourceLocationContextTransferMalformed)

int main(void)
{
    UmiSourceLocationContext value;
    umi_source_location_context_init(&value);
    value.workspace_id[0] = 's';
    value.file_path[0] = 's';
    value.symbol[0] = 's';
    value.line = (uint32_t)17U;
    value.column = (uint32_t)17U;
    value.selection_length = (uint32_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_source_location_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiSourceLocationContextTransferCases(&value) != 0) return 1;

    return 0;
}
