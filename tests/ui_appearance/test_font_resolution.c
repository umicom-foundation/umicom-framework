/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_font_resolution.c
 *
 * PURPOSE:
 *   Verify record the winning family and fallback depth selected for a semantic font stack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_resolution.h"
int main(void) {
    UmiAppearanceFontResolution item;
    if (umi_appearance_font_resolution_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_font_resolution_is_valid(&item)) return 2;
    if (umi_appearance_font_resolution_choose(&item,"","font.fallback")!=UMI_STATUS_OK || item.fallback_depth!=1U) return 3;
    return 0;
}
