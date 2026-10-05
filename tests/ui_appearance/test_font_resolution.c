/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_font_resolution.c
 *
 * PURPOSE:
 *   Verify record the winning family and fallback depth selected for a semantic font stack.
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
#include "umicom/ui/appearance/font_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/font_resolution.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceFontResolutionTransferEqual(const UmiAppearanceFontResolution *a, const UmiAppearanceFontResolution *b)
{
    return strcmp(a->stack_id, b->stack_id) == 0 &&
        strcmp(a->resolved_family_id, b->resolved_family_id) == 0 &&
        a->fallback_depth == b->fallback_depth &&
        a->available == b->available;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceFontResolutionTransferTails(UmiAppearanceFontResolution *value)
{
    (void)value;
    {
        size_t used = strlen(value->stack_id) + 1U;
        memset(value->stack_id + used, 0xa5, sizeof(value->stack_id) - used);
    }
    {
        size_t used = strlen(value->resolved_family_id) + 1U;
        memset(value->resolved_family_id + used, 0xa5, sizeof(value->resolved_family_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceFontResolutionTransferMalformed(const UmiAppearanceFontResolution *sample)
{
    (void)sample;
    {
        UmiAppearanceFontResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.stack_id, 'x', sizeof(invalid.stack_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_font_resolution_is_valid(&invalid)) ||
            umi_appearance_font_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated stack_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceFontResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.resolved_family_id, 'x', sizeof(invalid.resolved_family_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_font_resolution_is_valid(&invalid)) ||
            umi_appearance_font_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated resolved_family_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceFontResolutionTransferCases, UmiAppearanceFontResolution,
    umi_appearance_font_resolution_archive_encode, umi_appearance_font_resolution_archive_decode,
    UmiAppearanceFontResolutionTransferEqual, UmiAppearanceFontResolutionTransferTails, UmiAppearanceFontResolutionTransferMalformed)

int main(void) {
    UmiAppearanceFontResolution item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_font_resolution_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_font_resolution_is_valid(&item)) return 2;
    if (UmiAppearanceFontResolutionTransferCases(&item) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_font_resolution_choose(&item,"","font.fallback")!=UMI_STATUS_OK || item.fallback_depth!=1U) return 3;
    return 0;
}
