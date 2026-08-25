#include <stdio.h>
#include "umicom/ui/workstation/application_surface_set.h"
int main(void) {
    UmiWsApplicationSurfaceSet s;
    UmiWsSurfaceDescriptor d;
    if (umi_ws_application_surface_set_init(&s, "desktop", UMI_WS_DOMAIN_GENERIC) != UMI_STATUS_OK) return 1;
    if (umi_ws_application_surface_set_add(&s, "studio.editor") != UMI_STATUS_OK) return 2;
    if (!umi_ws_application_surface_set_contains(&s, "studio.editor")) return 3;
    if (umi_ws_surface_descriptor_init(&d, "trader.chart", "Chart", UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) != UMI_STATUS_OK) return 4;
    if (!umi_ws_application_surface_set_can_host(&s, &d)) return 5;
    puts("application surface set: ok");
    return 0;
}
