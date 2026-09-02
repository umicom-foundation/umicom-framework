/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_reduced_motion_mode.c
 *
 * PURPOSE:
 *   Verify resolve reduced-motion presentation requirements from user and system accessibility settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/reduced_motion_mode.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceReducedMotionMode item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_reduced_motion_mode_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_reduced_motion_mode_is_valid(&item)) return 2;
    return 0;
}
