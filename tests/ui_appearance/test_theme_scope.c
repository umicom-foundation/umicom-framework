/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_scope.c
 *
 * PURPOSE:
 *   Verify describe the semantic scope at which a theme override is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_scope.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceThemeScope item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_scope_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_theme_scope_is_valid(&item)) return 2;
    return 0;
}
