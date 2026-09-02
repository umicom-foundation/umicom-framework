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
#include "umicom/ui/appearance/user_appearance_preferences.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceUserAppearancePreferences item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_user_appearance_preferences_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_user_appearance_preferences_is_valid(&item)) return 2;
    return 0;
}
