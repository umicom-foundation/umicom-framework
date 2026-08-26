/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_profile.c
 *
 * PURPOSE:
 *   Verify capture a resolved user/application appearance profile shared by every renderer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_profile.h"
int main(void) {
    UmiAppearanceAppearanceProfile item;
    if (umi_appearance_profile_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_profile_is_valid(&item)) return 2;
    return 0;
}
