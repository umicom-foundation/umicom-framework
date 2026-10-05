/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_resolution.c
 *
 * PURPOSE:
 *   Verify record deterministic system/application/workspace/component theme resolution evidence.
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
#include "umicom/ui/appearance/theme_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/theme_resolution.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceThemeResolutionTransferEqual(const UmiAppearanceThemeResolution *a, const UmiAppearanceThemeResolution *b)
{
    return strcmp(a->requested_pack_id, b->requested_pack_id) == 0 &&
        strcmp(a->resolved_pack_id, b->resolved_pack_id) == 0 &&
        a->winning_scope == b->winning_scope &&
        a->inherited_layers == b->inherited_layers;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceThemeResolutionTransferTails(UmiAppearanceThemeResolution *value)
{
    (void)value;
    {
        size_t used = strlen(value->requested_pack_id) + 1U;
        memset(value->requested_pack_id + used, 0xa5, sizeof(value->requested_pack_id) - used);
    }
    {
        size_t used = strlen(value->resolved_pack_id) + 1U;
        memset(value->resolved_pack_id + used, 0xa5, sizeof(value->resolved_pack_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceThemeResolutionTransferMalformed(const UmiAppearanceThemeResolution *sample)
{
    (void)sample;
    {
        UmiAppearanceThemeResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.requested_pack_id, 'x', sizeof(invalid.requested_pack_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_theme_resolution_is_valid(&invalid)) ||
            umi_appearance_theme_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated requested_pack_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceThemeResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.resolved_pack_id, 'x', sizeof(invalid.resolved_pack_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_theme_resolution_is_valid(&invalid)) ||
            umi_appearance_theme_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated resolved_pack_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceThemeResolutionTransferCases, UmiAppearanceThemeResolution,
    umi_appearance_theme_resolution_archive_encode, umi_appearance_theme_resolution_archive_decode,
    UmiAppearanceThemeResolutionTransferEqual, UmiAppearanceThemeResolutionTransferTails, UmiAppearanceThemeResolutionTransferMalformed)

int main(void) {
    UmiAppearanceThemeResolution item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_resolution_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_theme_resolution_is_valid(&item)) return 2;
    if (UmiAppearanceThemeResolutionTransferCases(&item) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_resolution_choose(&item,"system","app","workspace","component")!=UMI_STATUS_OK || item.winning_scope!=UMI_APPEARANCE_SCOPE_COMPONENT) return 3;
    return 0;
}
