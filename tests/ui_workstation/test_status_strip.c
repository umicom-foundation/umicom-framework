/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_status_strip.c
 *
 * PURPOSE:
 *   Implement the test status strip behavior for
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
#include "umicom/ui/workstation/status_strip.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsStatusStrip s;
    umi_ws_status_strip_init(&s);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_status_strip_set(&s, "mode", "PAPER MODE", UMI_UI_SEVERITY_WARNING, false) != UMI_STATUS_OK) return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_status_strip_find(&s, "mode") == NULL) return 2;
    puts("status strip: ok");
    return 0;
}
