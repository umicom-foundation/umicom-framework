/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_palette_resolution.c
 *
 * PURPOSE:
 *   Verify record the winning token for a semantic palette role after scope precedence is applied.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/palette_resolution.h"
int main(void) {
    UmiAppearancePaletteResolution item;
    if (umi_appearance_palette_resolution_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_palette_resolution_is_valid(&item)) return 2;
    if (umi_appearance_palette_resolution_override(&item,"studio.accent",UMI_APPEARANCE_SCOPE_APPLICATION)!=UMI_STATUS_OK || item.winning_scope!=UMI_APPEARANCE_SCOPE_APPLICATION) return 3;
    return 0;
}
