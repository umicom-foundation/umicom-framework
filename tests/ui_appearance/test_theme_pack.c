/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_pack.c
 *
 * PURPOSE:
 *   Verify describe a versionable semantic theme pack without toolkit CSS or widget classes.
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
#include "umicom/ui/appearance/theme_pack.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/theme_pack.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceThemePackTransferEqual(const UmiAppearanceThemePack *a, const UmiAppearanceThemePack *b)
{
    return strcmp(a->pack_id, b->pack_id) == 0 &&
        strcmp(a->parent_pack_id, b->parent_pack_id) == 0 &&
        strcmp(a->brand_id, b->brand_id) == 0 &&
        strcmp(a->token_set_id, b->token_set_id) == 0 &&
        a->mode == b->mode &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceThemePackTransferTails(UmiAppearanceThemePack *value)
{
    (void)value;
    {
        size_t used = strlen(value->pack_id) + 1U;
        memset(value->pack_id + used, 0xa5, sizeof(value->pack_id) - used);
    }
    {
        size_t used = strlen(value->parent_pack_id) + 1U;
        memset(value->parent_pack_id + used, 0xa5, sizeof(value->parent_pack_id) - used);
    }
    {
        size_t used = strlen(value->brand_id) + 1U;
        memset(value->brand_id + used, 0xa5, sizeof(value->brand_id) - used);
    }
    {
        size_t used = strlen(value->token_set_id) + 1U;
        memset(value->token_set_id + used, 0xa5, sizeof(value->token_set_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceThemePackTransferMalformed(const UmiAppearanceThemePack *sample)
{
    (void)sample;
    {
        UmiAppearanceThemePack invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.pack_id, 'x', sizeof(invalid.pack_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_theme_pack_is_valid(&invalid)) ||
            umi_appearance_theme_pack_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated pack_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceThemePack invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_pack_id, 'x', sizeof(invalid.parent_pack_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_theme_pack_is_valid(&invalid)) ||
            umi_appearance_theme_pack_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_pack_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceThemePack invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.brand_id, 'x', sizeof(invalid.brand_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_theme_pack_is_valid(&invalid)) ||
            umi_appearance_theme_pack_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated brand_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceThemePack invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.token_set_id, 'x', sizeof(invalid.token_set_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_theme_pack_is_valid(&invalid)) ||
            umi_appearance_theme_pack_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated token_set_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceThemePackTransferCases, UmiAppearanceThemePack,
    umi_appearance_theme_pack_archive_encode, umi_appearance_theme_pack_archive_decode,
    UmiAppearanceThemePackTransferEqual, UmiAppearanceThemePackTransferTails, UmiAppearanceThemePackTransferMalformed)

int main(void) {
    UmiAppearanceThemePack item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_pack_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_theme_pack_is_valid(&item)) return 2;
    if (UmiAppearanceThemePackTransferCases(&item) != 0) return 1;

    return 0;
}
