/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_shortcut_presentation.c
 *
 * PURPOSE:
 *   Verify describe platform-neutral command shortcut hints for menus, toolbars and palettes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/shortcut_presentation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceShortcutPresentation item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_shortcut_presentation_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_shortcut_presentation_is_valid(&item)) return 2;
    return 0;
}
