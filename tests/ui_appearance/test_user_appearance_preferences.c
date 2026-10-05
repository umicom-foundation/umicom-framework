/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_user_appearance_preferences.c
 *
 * PURPOSE:
 *   Verify capture user-selected theme, density, motion and text-scale preferences independently of toolkit settings.
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
#include "umicom/ui/appearance/user_appearance_preferences.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/user_appearance_preferences.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceUserAppearancePreferencesTransferEqual(const UmiAppearanceUserAppearancePreferences *a, const UmiAppearanceUserAppearancePreferences *b)
{
    return strcmp(a->user_scope_id, b->user_scope_id) == 0 &&
        a->theme_mode == b->theme_mode &&
        a->density == b->density &&
        a->text_scale == b->text_scale &&
        a->follow_system_theme == b->follow_system_theme &&
        a->reduced_motion == b->reduced_motion &&
        a->high_contrast == b->high_contrast;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceUserAppearancePreferencesTransferTails(UmiAppearanceUserAppearancePreferences *value)
{
    (void)value;
    {
        size_t used = strlen(value->user_scope_id) + 1U;
        memset(value->user_scope_id + used, 0xa5, sizeof(value->user_scope_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceUserAppearancePreferencesTransferMalformed(const UmiAppearanceUserAppearancePreferences *sample)
{
    (void)sample;
    {
        UmiAppearanceUserAppearancePreferences invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.user_scope_id, 'x', sizeof(invalid.user_scope_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_user_appearance_preferences_is_valid(&invalid)) ||
            umi_appearance_user_appearance_preferences_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated user_scope_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceUserAppearancePreferencesTransferCases, UmiAppearanceUserAppearancePreferences,
    umi_appearance_user_appearance_preferences_archive_encode, umi_appearance_user_appearance_preferences_archive_decode,
    UmiAppearanceUserAppearancePreferencesTransferEqual, UmiAppearanceUserAppearancePreferencesTransferTails, UmiAppearanceUserAppearancePreferencesTransferMalformed)

int main(void) {
    UmiAppearanceUserAppearancePreferences item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_user_appearance_preferences_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_user_appearance_preferences_is_valid(&item)) return 2;
    if (UmiAppearanceUserAppearancePreferencesTransferCases(&item) != 0) return 1;

    return 0;
}
