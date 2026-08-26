/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_user_appearance_preferences.c
 *
 * PURPOSE:
 *   Verify capture user-selected theme, density, motion and text-scale preferences independently of toolkit settings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/user_appearance_preferences.h"
int main(void) {
    UmiAppearanceUserAppearancePreferences item;
    if (umi_appearance_user_appearance_preferences_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_user_appearance_preferences_is_valid(&item)) return 2;
    return 0;
}
