/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_variant.c
 *
 * PURPOSE:
 *   Verify bind a semantic theme pack to light, dark or high-contrast presentation mode.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_variant.h"
int main(void) {
    UmiAppearanceThemeVariant item;
    if (umi_appearance_theme_variant_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_theme_variant_is_valid(&item)) return 2;
    return 0;
}
