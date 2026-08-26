/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_pack.c
 *
 * PURPOSE:
 *   Verify describe a versionable semantic theme pack without toolkit CSS or widget classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_pack.h"
int main(void) {
    UmiAppearanceThemePack item;
    if (umi_appearance_theme_pack_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_theme_pack_is_valid(&item)) return 2;
    return 0;
}
