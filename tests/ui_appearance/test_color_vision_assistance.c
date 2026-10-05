/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_color_vision_assistance.c
 *
 * PURPOSE:
 *   Verify require non-colour cues when semantic state could otherwise depend on colour perception alone.
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
#include "umicom/ui/appearance/color_vision_assistance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/color_vision_assistance.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceColorVisionAssistanceTransferEqual(const UmiAppearanceColorVisionAssistance *a, const UmiAppearanceColorVisionAssistance *b)
{
    return strcmp(a->profile_id, b->profile_id) == 0 &&
        a->require_icons == b->require_icons &&
        a->require_text_labels == b->require_text_labels &&
        a->require_patterns == b->require_patterns &&
        a->avoid_red_green_only == b->avoid_red_green_only;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceColorVisionAssistanceTransferTails(UmiAppearanceColorVisionAssistance *value)
{
    (void)value;
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceColorVisionAssistanceTransferMalformed(const UmiAppearanceColorVisionAssistance *sample)
{
    (void)sample;
    {
        UmiAppearanceColorVisionAssistance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_color_vision_assistance_is_valid(&invalid)) ||
            umi_appearance_color_vision_assistance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceColorVisionAssistanceTransferCases, UmiAppearanceColorVisionAssistance,
    umi_appearance_color_vision_assistance_archive_encode, umi_appearance_color_vision_assistance_archive_decode,
    UmiAppearanceColorVisionAssistanceTransferEqual, UmiAppearanceColorVisionAssistanceTransferTails, UmiAppearanceColorVisionAssistanceTransferMalformed)

int main(void) {
    UmiAppearanceColorVisionAssistance item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_color_vision_assistance_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_color_vision_assistance_is_valid(&item)) return 2;
    if (UmiAppearanceColorVisionAssistanceTransferCases(&item) != 0) return 1;

    return 0;
}
