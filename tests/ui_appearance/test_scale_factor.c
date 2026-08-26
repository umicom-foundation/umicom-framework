/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_scale_factor.c
 *
 * PURPOSE:
 *   Verify represent a bounded effective UI scale factor with independent OS and user contributions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/scale_factor.h"
int main(void) {
    UmiAppearanceScaleFactor item;
    if (umi_appearance_scale_factor_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_scale_factor_is_valid(&item)) return 2;
    return 0;
}
