/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_high_contrast_mode.c
 *
 * PURPOSE:
 *   Verify represent high-contrast presentation requirements layered over the canonical Design System.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/high_contrast_mode.h"
int main(void) {
    UmiAppearanceHighContrastMode item;
    if (umi_appearance_high_contrast_mode_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_high_contrast_mode_is_valid(&item)) return 2;
    return 0;
}
