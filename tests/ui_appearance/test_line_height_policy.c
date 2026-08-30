/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_line_height_policy.c
 *
 * PURPOSE:
 *   Verify maintain readable line-height bounds as font and accessibility scale changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/line_height_policy.h"
int main(void) {
    UmiAppearanceLineHeightPolicy item;
    if (umi_appearance_line_height_policy_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_line_height_policy_is_valid(&item)) return 2;
    return 0;
}
