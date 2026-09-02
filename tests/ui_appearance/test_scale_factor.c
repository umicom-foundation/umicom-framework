/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_scale_factor.c
 *
 * PURPOSE:
 *   Verify represent a bounded effective UI scale factor with independent OS and user contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/scale_factor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceScaleFactor item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_scale_factor_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_scale_factor_is_valid(&item)) return 2;
    return 0;
}
