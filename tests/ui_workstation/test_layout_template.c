/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_template.c
 *
 * PURPOSE:
 *   Implement the test layout template behavior for
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
#include "umicom/ui/workstation/layout_template.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutTemplate l;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_template_init(&l, "studio.code", "Code", UMI_WS_DOMAIN_STUDIO) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_template_add(&l, "studio.editor", UMI_WS_DOCK_CENTRE, 0, 1, 2.0) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_layout_template_find(&l, "studio.editor") == NULL) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_layout_template_fingerprint(&l) == 0U) return 4;
    puts("layout template: ok");
    return 0;
}
