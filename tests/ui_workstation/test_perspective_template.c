/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_perspective_template.c
 *
 * PURPOSE:
 *   Implement the test perspective template behavior for
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
#include "umicom/ui/workstation/perspective_template.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsPerspectiveTemplate p;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_template_init(&p, "studio.debug", "Debug", UMI_WS_DOMAIN_STUDIO) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_template_set_default_layout(&p, "studio.debug.desktop") != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_perspective_template_add_surface(&p, "debug.variables") != UMI_STATUS_OK) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (p.preferred_surface_count != 1U) return 4;
    puts("perspective template: ok");
    return 0;
}
