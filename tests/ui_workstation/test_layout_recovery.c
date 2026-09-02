/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_recovery.c
 *
 * PURPOSE:
 *   Implement the test layout recovery behavior for
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
#include "umicom/ui/workstation/layout_recovery.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsMonitorTopology t;
    UmiWsMonitor m = {"primary", {0,0,1920,1080}, 100, true};
    UmiWsFloatingWindow w;
    UmiWsLayoutRecoveryResult r;
    umi_ws_monitor_topology_init(&t);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_monitor_topology_add(&t, &m) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_floating_window_init(&w, "w", "panel", (UmiUiRect){3000,0,900,700}) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_floating_window_assign_monitor(&w, "missing") != UMI_STATUS_OK) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_layout_recover_window(&w, &t, &r) != UMI_STATUS_OK) return 4;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!r.monitor_changed || strcmp(r.chosen_monitor_id, "primary") != 0) return 5;
    puts("layout recovery: ok");
    return 0;
}
