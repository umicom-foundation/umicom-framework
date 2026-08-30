/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_palette_override.c
 *
 * PURPOSE:
 *   Verify describe a scoped semantic palette override without embedding literal renderer colours.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/palette_override.h"
int main(void) {
    UmiAppearancePaletteOverride item;
    if (umi_appearance_palette_override_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_palette_override_is_valid(&item)) return 2;
    return 0;
}
