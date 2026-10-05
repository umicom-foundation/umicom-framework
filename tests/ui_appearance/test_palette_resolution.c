/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_palette_resolution.c
 *
 * PURPOSE:
 *   Verify record the winning token for a semantic palette role after scope precedence is applied.
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
#include "umicom/ui/appearance/palette_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/palette_resolution.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearancePaletteResolutionTransferEqual(const UmiAppearancePaletteResolution *a, const UmiAppearancePaletteResolution *b)
{
    return strcmp(a->role_id, b->role_id) == 0 &&
        strcmp(a->base_token_id, b->base_token_id) == 0 &&
        strcmp(a->resolved_token_id, b->resolved_token_id) == 0 &&
        a->winning_scope == b->winning_scope;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearancePaletteResolutionTransferTails(UmiAppearancePaletteResolution *value)
{
    (void)value;
    {
        size_t used = strlen(value->role_id) + 1U;
        memset(value->role_id + used, 0xa5, sizeof(value->role_id) - used);
    }
    {
        size_t used = strlen(value->base_token_id) + 1U;
        memset(value->base_token_id + used, 0xa5, sizeof(value->base_token_id) - used);
    }
    {
        size_t used = strlen(value->resolved_token_id) + 1U;
        memset(value->resolved_token_id + used, 0xa5, sizeof(value->resolved_token_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearancePaletteResolutionTransferMalformed(const UmiAppearancePaletteResolution *sample)
{
    (void)sample;
    {
        UmiAppearancePaletteResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.role_id, 'x', sizeof(invalid.role_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_palette_resolution_is_valid(&invalid)) ||
            umi_appearance_palette_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated role_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearancePaletteResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.base_token_id, 'x', sizeof(invalid.base_token_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_palette_resolution_is_valid(&invalid)) ||
            umi_appearance_palette_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated base_token_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearancePaletteResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.resolved_token_id, 'x', sizeof(invalid.resolved_token_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_palette_resolution_is_valid(&invalid)) ||
            umi_appearance_palette_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated resolved_token_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearancePaletteResolutionTransferCases, UmiAppearancePaletteResolution,
    umi_appearance_palette_resolution_archive_encode, umi_appearance_palette_resolution_archive_decode,
    UmiAppearancePaletteResolutionTransferEqual, UmiAppearancePaletteResolutionTransferTails, UmiAppearancePaletteResolutionTransferMalformed)

int main(void) {
    UmiAppearancePaletteResolution item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_palette_resolution_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_palette_resolution_is_valid(&item)) return 2;
    if (UmiAppearancePaletteResolutionTransferCases(&item) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_palette_resolution_override(&item,"studio.accent",UMI_APPEARANCE_SCOPE_APPLICATION)!=UMI_STATUS_OK || item.winning_scope!=UMI_APPEARANCE_SCOPE_APPLICATION) return 3;
    return 0;
}
