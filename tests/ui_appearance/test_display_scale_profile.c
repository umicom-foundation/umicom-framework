/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_display_scale_profile.c
 *
 * PURPOSE:
 *   Verify combine display DPI, operating-system scale and user accessibility scale into one profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/display_scale_profile.h"
int main(void) {
    UmiAppearanceDisplayScaleProfile item;
    if (umi_appearance_display_scale_profile_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_display_scale_profile_is_valid(&item)) return 2;
    item.os_scale=1.5; item.user_scale=2.0; if(umi_appearance_display_scale_profile_resolve(&item)!=UMI_STATUS_OK || item.effective_scale!=3.0) return 3;
    return 0;
}
