/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_resolution.c
 *
 * PURPOSE:
 *   Verify record deterministic system/application/workspace/component theme resolution evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceThemeResolution item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_resolution_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_theme_resolution_is_valid(&item)) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_resolution_choose(&item,"system","app","workspace","component")!=UMI_STATUS_OK || item.winning_scope!=UMI_APPEARANCE_SCOPE_COMPONENT) return 3;
    return 0;
}
