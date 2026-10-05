/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_high_contrast_mode.c
 *
 * PURPOSE:
 *   Verify represent high-contrast presentation requirements layered over the canonical Design System.
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
#include "umicom/ui/appearance/high_contrast_mode.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/high_contrast_mode.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceHighContrastModeTransferEqual(const UmiAppearanceHighContrastMode *a, const UmiAppearanceHighContrastMode *b)
{
    return strcmp(a->mode_id, b->mode_id) == 0 &&
        a->enabled == b->enabled &&
        a->force_visible_borders == b->force_visible_borders &&
        a->force_focus_outline == b->force_focus_outline &&
        a->minimum_border_width == b->minimum_border_width;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceHighContrastModeTransferTails(UmiAppearanceHighContrastMode *value)
{
    (void)value;
    {
        size_t used = strlen(value->mode_id) + 1U;
        memset(value->mode_id + used, 0xa5, sizeof(value->mode_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceHighContrastModeTransferMalformed(const UmiAppearanceHighContrastMode *sample)
{
    (void)sample;
    {
        UmiAppearanceHighContrastMode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.mode_id, 'x', sizeof(invalid.mode_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_high_contrast_mode_is_valid(&invalid)) ||
            umi_appearance_high_contrast_mode_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated mode_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceHighContrastModeTransferCases, UmiAppearanceHighContrastMode,
    umi_appearance_high_contrast_mode_archive_encode, umi_appearance_high_contrast_mode_archive_decode,
    UmiAppearanceHighContrastModeTransferEqual, UmiAppearanceHighContrastModeTransferTails, UmiAppearanceHighContrastModeTransferMalformed)

int main(void) {
    UmiAppearanceHighContrastMode item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_high_contrast_mode_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_high_contrast_mode_is_valid(&item)) return 2;
    if (UmiAppearanceHighContrastModeTransferCases(&item) != 0) return 1;

    return 0;
}
