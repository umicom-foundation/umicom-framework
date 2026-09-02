/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_monitor_topology.c
 *
 * PURPOSE:
 *   Implement the test monitor topology behavior for
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
#include <string.h>
#include "umicom/ui/workstation/monitor_topology.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsMonitorTopology t;
    UmiWsMonitor m = {"monitor.1", {0,0,1920,1080}, 100, true};
    umi_ws_monitor_topology_init(&t);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_monitor_topology_add(&t, &m) != UMI_STATUS_OK) return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ws_monitor_topology_find(&t, "monitor.1") == NULL) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(umi_ws_monitor_topology_fallback(&t)->monitor_id, "monitor.1") != 0) return 3;
    puts("monitor topology: ok");
    return 0;
}
