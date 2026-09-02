/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_variant.c
 *
 * PURPOSE:
 *   Verify bind a semantic theme pack to light, dark or high-contrast presentation mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_variant.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceThemeVariant item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_variant_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_theme_variant_is_valid(&item)) return 2;
    return 0;
}
