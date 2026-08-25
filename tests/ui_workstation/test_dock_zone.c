#include <stdio.h>
#include "umicom/ui/workstation/dock_zone.h"
int main(void) {
    UmiWsDockZone z;
    if (umi_ws_dock_zone_classify(0.05, 0.5, 0.2) != UMI_WS_DOCK_LEFT) return 1;
    if (umi_ws_dock_zone_classify(0.5, 0.5, 0.2) != UMI_WS_DOCK_CENTRE) return 2;
    if (umi_ws_dock_zone_init(&z, UMI_WS_DOCK_BOTTOM, 0.25) != UMI_STATUS_OK) return 3;
    puts("dock zone: ok");
    return 0;
}
