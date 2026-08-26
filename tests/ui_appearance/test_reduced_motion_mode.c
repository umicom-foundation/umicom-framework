/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_reduced_motion_mode.c
 *
 * PURPOSE:
 *   Verify resolve reduced-motion presentation requirements from user and system accessibility settings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/reduced_motion_mode.h"
int main(void) {
    UmiAppearanceReducedMotionMode item;
    if (umi_appearance_reduced_motion_mode_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_reduced_motion_mode_is_valid(&item)) return 2;
    return 0;
}
