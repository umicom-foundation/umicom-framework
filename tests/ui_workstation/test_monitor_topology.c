#include <stdio.h>
#include <string.h>
#include "umicom/ui/workstation/monitor_topology.h"
int main(void) {
    UmiWsMonitorTopology t;
    UmiWsMonitor m = {"monitor.1", {0,0,1920,1080}, 100, true};
    umi_ws_monitor_topology_init(&t);
    if (umi_ws_monitor_topology_add(&t, &m) != UMI_STATUS_OK) return 1;
    if (umi_ws_monitor_topology_find(&t, "monitor.1") == NULL) return 2;
    if (strcmp(umi_ws_monitor_topology_fallback(&t)->monitor_id, "monitor.1") != 0) return 3;
    puts("monitor topology: ok");
    return 0;
}
