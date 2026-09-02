/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_maximize_mode.c
 *
 * PURPOSE:
 *   Implement the test maximize mode behavior for
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
#include "umicom/ui/workstation/maximize_mode.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsMaximizeMode m;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_maximize_mode_activate(&m, "chart") != UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_ws_maximize_mode_matches(&m, "chart")) return 2;
    umi_ws_maximize_mode_restore(&m);
    /* Apply this operation only while the related capability or state is available. */
    if (m.active) return 3;
    puts("maximize mode: ok");
    return 0;
}
