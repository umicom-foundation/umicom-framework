#include <stdio.h>
#include "umicom/ui/workstation/surface_catalogue.h"
int main(void) {
    UmiWsSurfaceCatalogue c;
    UmiWsSurfaceDescriptor d;
    umi_ws_surface_catalogue_init(&c);
    if (umi_ws_surface_descriptor_init(&d, "trader.chart", "Chart", UMI_WS_DOMAIN_TRADER, UMI_WS_SURFACE_CHART) != UMI_STATUS_OK) return 1;
    if (umi_ws_surface_catalogue_upsert(&c, &d) != UMI_STATUS_OK) return 2;
    if (umi_ws_surface_catalogue_find(&c, "trader.chart") == NULL) return 3;
    if (umi_ws_surface_catalogue_count_domain(&c, UMI_WS_DOMAIN_TRADER) != 1U) return 4;
    puts("surface catalogue: ok");
    return 0;
}
