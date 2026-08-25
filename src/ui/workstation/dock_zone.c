/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/dock_zone.c
 *
 * PURPOSE:
 *   Implement screen-relative docking zone classification used by mouse and keyboard layout operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/dock_zone.h"

UmiWsDockRegion umi_ws_dock_zone_classify(double x_ratio, double y_ratio, double edge_fraction) {
    double edge = umi_ws_clamp_unit(edge_fraction);
    double x = umi_ws_clamp_unit(x_ratio);
    double y = umi_ws_clamp_unit(y_ratio);
    if (x <= edge) return UMI_WS_DOCK_LEFT;
    if (x >= 1.0 - edge) return UMI_WS_DOCK_RIGHT;
    if (y <= edge) return UMI_WS_DOCK_TOP;
    if (y >= 1.0 - edge) return UMI_WS_DOCK_BOTTOM;
    return UMI_WS_DOCK_CENTRE;
}

UmiStatus umi_ws_dock_zone_init(UmiWsDockZone *zone, UmiWsDockRegion region, double edge_fraction) {
    if (zone == NULL || region < UMI_WS_DOCK_LEFT || region > UMI_WS_DOCK_FLOATING) return UMI_STATUS_INVALID_ARGUMENT;
    zone->region = region;
    zone->edge_fraction = umi_ws_clamp_unit(edge_fraction);
    return UMI_STATUS_OK;
}
