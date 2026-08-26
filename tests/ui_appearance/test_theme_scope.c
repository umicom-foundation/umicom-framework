/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_scope.c
 *
 * PURPOSE:
 *   Verify describe the semantic scope at which a theme override is applied.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_scope.h"
int main(void) {
    UmiAppearanceThemeScope item;
    if (umi_appearance_theme_scope_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_theme_scope_is_valid(&item)) return 2;
    return 0;
}
