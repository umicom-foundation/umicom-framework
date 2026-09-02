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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearancePaletteOverride item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_palette_override_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_palette_override_is_valid(&item)) return 2;
    return 0;
}
