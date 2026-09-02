/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_palette_resolution.c
 *
 * PURPOSE:
 *   Verify record the winning token for a semantic palette role after scope precedence is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/palette_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearancePaletteResolution item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_palette_resolution_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_palette_resolution_is_valid(&item)) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_palette_resolution_override(&item,"studio.accent",UMI_APPEARANCE_SCOPE_APPLICATION)!=UMI_STATUS_OK || item.winning_scope!=UMI_APPEARANCE_SCOPE_APPLICATION) return 3;
    return 0;
}
