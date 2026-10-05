/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_display_scale_profile.c
 *
 * PURPOSE:
 *   Verify combine display DPI, operating-system scale and user accessibility scale into one profile.
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
#include "umicom/ui/appearance/display_scale_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/display_scale_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceDisplayScaleProfileTransferEqual(const UmiAppearanceDisplayScaleProfile *a, const UmiAppearanceDisplayScaleProfile *b)
{
    return strcmp(a->display_id, b->display_id) == 0 &&
        a->dpi == b->dpi &&
        a->os_scale == b->os_scale &&
        a->user_scale == b->user_scale &&
        a->effective_scale == b->effective_scale;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceDisplayScaleProfileTransferTails(UmiAppearanceDisplayScaleProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->display_id) + 1U;
        memset(value->display_id + used, 0xa5, sizeof(value->display_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceDisplayScaleProfileTransferMalformed(const UmiAppearanceDisplayScaleProfile *sample)
{
    (void)sample;
    {
        UmiAppearanceDisplayScaleProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_id, 'x', sizeof(invalid.display_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_display_scale_profile_is_valid(&invalid)) ||
            umi_appearance_display_scale_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceDisplayScaleProfileTransferCases, UmiAppearanceDisplayScaleProfile,
    umi_appearance_display_scale_profile_archive_encode, umi_appearance_display_scale_profile_archive_decode,
    UmiAppearanceDisplayScaleProfileTransferEqual, UmiAppearanceDisplayScaleProfileTransferTails, UmiAppearanceDisplayScaleProfileTransferMalformed)

int main(void) {
    UmiAppearanceDisplayScaleProfile item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_display_scale_profile_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_display_scale_profile_is_valid(&item)) return 2;
    if (UmiAppearanceDisplayScaleProfileTransferCases(&item) != 0) return 1;

    item.os_scale=1.5; item.user_scale=2.0; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_display_scale_profile_resolve(&item)!=UMI_STATUS_OK || item.effective_scale!=3.0) return 3;
    return 0;
}
