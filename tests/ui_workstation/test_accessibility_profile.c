/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_accessibility_profile.c
 *
 * PURPOSE:
 *   Implement the test accessibility profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/ui/workstation/accessibility_profile.h"
int main(void) {
    UmiWsAccessibilityProfile p = umi_ws_accessibility_profile_default(UMI_WS_DENSITY_TOUCH);
    if (!p.keyboard_docking || !p.require_non_colour_group_label) return 1;
    if (!umi_ws_accessibility_profile_hit_target_ok(&p, 48, 50)) return 2;
    if (umi_ws_accessibility_profile_hit_target_ok(&p, 20, 50)) return 3;
    puts("accessibility profile: ok");
    return 0;
}
