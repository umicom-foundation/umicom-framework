#include <stdio.h>
#include "umicom/ui/workstation/chart_surface.h"
int main(void) {
    UmiWsChartSurface c;
    if (umi_ws_chart_surface_init(&c, "chart.main", "ES · 1m") != UMI_STATUS_OK) return 1;
    if (umi_ws_chart_surface_set_context_group(&c, "group.1") != UMI_STATUS_OK) return 2;
    if (umi_ws_chart_surface_set_interval(&c, 300) != UMI_STATUS_OK) return 3;
    if (umi_ws_chart_surface_set_zoom(&c, 50.0) != 20.0) return 4;
    puts("chart surface: ok");
    return 0;
}
