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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceHighContrastMode item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_high_contrast_mode_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_high_contrast_mode_is_valid(&item)) return 2;
    return 0;
}
