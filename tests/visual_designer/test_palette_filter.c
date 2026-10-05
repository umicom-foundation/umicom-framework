/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_palette_filter.c
 *
 * PURPOSE:
 *   Validate filter the component palette by text, category and capability.
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
#include "umicom/designer/visual_designer/palette_filter.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/palette_filter.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadPaletteFilterTransferEqual(const UmiRadPaletteFilter *a, const UmiRadPaletteFilter *b)
{
    return strcmp(a->query, b->query) == 0 &&
        strcmp(a->category, b->category) == 0 &&
        strcmp(a->capability, b->capability) == 0 &&
        a->favourites_only == b->favourites_only;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadPaletteFilterTransferTails(UmiRadPaletteFilter *value)
{
    (void)value;
    {
        size_t used = strlen(value->query) + 1U;
        memset(value->query + used, 0xa5, sizeof(value->query) - used);
    }
    {
        size_t used = strlen(value->category) + 1U;
        memset(value->category + used, 0xa5, sizeof(value->category) - used);
    }
    {
        size_t used = strlen(value->capability) + 1U;
        memset(value->capability + used, 0xa5, sizeof(value->capability) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadPaletteFilterTransferMalformed(const UmiRadPaletteFilter *sample)
{
    (void)sample;
    {
        UmiRadPaletteFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.query, 'x', sizeof(invalid.query));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_palette_filter_is_valid(&invalid)) ||
            umi_rad_palette_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated query was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPaletteFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.category, 'x', sizeof(invalid.category));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_palette_filter_is_valid(&invalid)) ||
            umi_rad_palette_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated category was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPaletteFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.capability, 'x', sizeof(invalid.capability));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_palette_filter_is_valid(&invalid)) ||
            umi_rad_palette_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated capability was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadPaletteFilterTransferCases, UmiRadPaletteFilter,
    umi_rad_palette_filter_archive_encode, umi_rad_palette_filter_archive_decode,
    UmiRadPaletteFilterTransferEqual, UmiRadPaletteFilterTransferTails, UmiRadPaletteFilterTransferMalformed)

int main(void){UmiRadPaletteFilter item;CHECK(umi_rad_palette_filter_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_palette_filter_is_valid(&item));
    if (UmiRadPaletteFilterTransferCases(&item) != 0) return 1;
return 0;}
