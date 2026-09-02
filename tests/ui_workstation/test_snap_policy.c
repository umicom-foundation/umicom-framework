/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_snap_policy.c
 *
 * PURPOSE:
 *   Implement the test snap policy behavior for
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
#include "umicom/ui/workstation/snap_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsSnapPolicy p = umi_ws_snap_policy_default(UMI_WS_DENSITY_COMPACT);
    /* Apply this branch only when its contract condition is satisfied. */
    if (p.grid_size != 6) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_snap_policy_coordinate(&p, 10) != 12) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ws_snap_policy_near_edge(&p, 105, 100)) return 3;
    puts("snap policy: ok");
    return 0;
}
