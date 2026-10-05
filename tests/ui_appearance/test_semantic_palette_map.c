/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_semantic_palette_map.c
 *
 * PURPOSE:
 *   Verify map a semantic colour role to a Design-System token identity.
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
#include "umicom/ui/appearance/semantic_palette_map.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/semantic_palette_map.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceSemanticPaletteMapTransferEqual(const UmiAppearanceSemanticPaletteMap *a, const UmiAppearanceSemanticPaletteMap *b)
{
    return strcmp(a->role_id, b->role_id) == 0 &&
        strcmp(a->token_id, b->token_id) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceSemanticPaletteMapTransferTails(UmiAppearanceSemanticPaletteMap *value)
{
    (void)value;
    {
        size_t used = strlen(value->role_id) + 1U;
        memset(value->role_id + used, 0xa5, sizeof(value->role_id) - used);
    }
    {
        size_t used = strlen(value->token_id) + 1U;
        memset(value->token_id + used, 0xa5, sizeof(value->token_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceSemanticPaletteMapTransferMalformed(const UmiAppearanceSemanticPaletteMap *sample)
{
    (void)sample;
    {
        UmiAppearanceSemanticPaletteMap invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.role_id, 'x', sizeof(invalid.role_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_semantic_palette_map_is_valid(&invalid)) ||
            umi_appearance_semantic_palette_map_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated role_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceSemanticPaletteMap invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.token_id, 'x', sizeof(invalid.token_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_semantic_palette_map_is_valid(&invalid)) ||
            umi_appearance_semantic_palette_map_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated token_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceSemanticPaletteMapTransferCases, UmiAppearanceSemanticPaletteMap,
    umi_appearance_semantic_palette_map_archive_encode, umi_appearance_semantic_palette_map_archive_decode,
    UmiAppearanceSemanticPaletteMapTransferEqual, UmiAppearanceSemanticPaletteMapTransferTails, UmiAppearanceSemanticPaletteMapTransferMalformed)

int main(void) {
    UmiAppearanceSemanticPaletteMap item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_semantic_palette_map_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_semantic_palette_map_is_valid(&item)) return 2;
    if (UmiAppearanceSemanticPaletteMapTransferCases(&item) != 0) return 1;

    return 0;
}
