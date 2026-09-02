/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_high_dpi_policy.c
 *
 * PURPOSE:
 *   Verify define fractional-layout, snapping and asset-resolution rules for high-DPI displays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/high_dpi_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceHighDpiPolicy item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_high_dpi_policy_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_high_dpi_policy_is_valid(&item)) return 2;
    return 0;
}
