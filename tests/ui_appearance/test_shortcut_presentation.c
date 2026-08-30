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
int main(void) {
    UmiAppearanceShortcutPresentation item;
    if (umi_appearance_shortcut_presentation_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_shortcut_presentation_is_valid(&item)) return 2;
    return 0;
}
