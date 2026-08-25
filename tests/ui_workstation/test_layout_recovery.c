#include <stdio.h>
#include <string.h>
#include "umicom/ui/workstation/layout_recovery.h"
int main(void) {
    UmiWsMonitorTopology t;
    UmiWsMonitor m = {"primary", {0,0,1920,1080}, 100, true};
    UmiWsFloatingWindow w;
    UmiWsLayoutRecoveryResult r;
    umi_ws_monitor_topology_init(&t);
    if (umi_ws_monitor_topology_add(&t, &m) != UMI_STATUS_OK) return 1;
    if (umi_ws_floating_window_init(&w, "w", "panel", (UmiUiRect){3000,0,900,700}) != UMI_STATUS_OK) return 2;
    if (umi_ws_floating_window_assign_monitor(&w, "missing") != UMI_STATUS_OK) return 3;
    if (umi_ws_layout_recover_window(&w, &t, &r) != UMI_STATUS_OK) return 4;
    if (!r.monitor_changed || strcmp(r.chosen_monitor_id, "primary") != 0) return 5;
    puts("layout recovery: ok");
    return 0;
}
