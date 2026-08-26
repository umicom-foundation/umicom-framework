/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_resolution.c
 *
 * PURPOSE:
 *   Verify record deterministic system/application/workspace/component theme resolution evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_resolution.h"
int main(void) {
    UmiAppearanceThemeResolution item;
    if (umi_appearance_theme_resolution_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_theme_resolution_is_valid(&item)) return 2;
    if (umi_appearance_theme_resolution_choose(&item,"system","app","workspace","component")!=UMI_STATUS_OK || item.winning_scope!=UMI_APPEARANCE_SCOPE_COMPONENT) return 3;
    return 0;
}
