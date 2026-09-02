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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsAccessibilityProfile p = umi_ws_accessibility_profile_default(UMI_WS_DENSITY_TOUCH);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!p.keyboard_docking || !p.require_non_colour_group_label) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ws_accessibility_profile_hit_target_ok(&p, 48, 50)) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_accessibility_profile_hit_target_ok(&p, 20, 50)) return 3;
    puts("accessibility profile: ok");
    return 0;
}
