/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_pack.c
 *
 * PURPOSE:
 *   Verify describe a versionable semantic theme pack without toolkit CSS or widget classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_pack.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceThemePack item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_pack_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_theme_pack_is_valid(&item)) return 2;
    return 0;
}
