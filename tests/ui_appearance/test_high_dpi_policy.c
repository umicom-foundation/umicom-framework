/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_high_dpi_policy.c
 *
 * PURPOSE:
 *   Verify define fractional-layout, snapping and asset-resolution rules for high-DPI displays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/high_dpi_policy.h"
int main(void) {
    UmiAppearanceHighDpiPolicy item;
    if (umi_appearance_high_dpi_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_high_dpi_policy_is_valid(&item)) return 2;
    return 0;
}
