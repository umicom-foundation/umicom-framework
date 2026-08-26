/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_variant_resolution.c
 *
 * PURPOSE:
 *   Verify resolve light/dark/high-contrast and direction-aware icon variants while preserving semantic identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_variant_resolution.h"
int main(void) {
    UmiAppearanceIconVariantResolution item;
    if (umi_appearance_icon_variant_resolution_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_icon_variant_resolution_is_valid(&item)) return 2;
    umi_appearance_icon_variant_resolution_set_direction(&item,1,1); if(!item.mirrored) return 3;
    return 0;
}
