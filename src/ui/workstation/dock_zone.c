/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/dock_zone.c
 *
 * PURPOSE:
 *   Implement screen-relative docking zone classification used by mouse and keyboard layout operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/dock_zone.h"

/*
 * Provide the ws dock zone classify operation used by this module and its client
 * applications.
 */
UmiWsDockRegion umi_ws_dock_zone_classify(double x_ratio, double y_ratio, double edge_fraction) {
    double edge = umi_ws_clamp_unit(edge_fraction);
    double x = umi_ws_clamp_unit(x_ratio);
    double y = umi_ws_clamp_unit(y_ratio);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (x <= edge) return UMI_WS_DOCK_LEFT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (x >= 1.0 - edge) return UMI_WS_DOCK_RIGHT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (y <= edge) return UMI_WS_DOCK_TOP;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (y >= 1.0 - edge) return UMI_WS_DOCK_BOTTOM;
    return UMI_WS_DOCK_CENTRE;
}

/*
 * Initialise ws dock zone from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ws_dock_zone_init(UmiWsDockZone *zone, UmiWsDockRegion region, double edge_fraction) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (zone == NULL || region < UMI_WS_DOCK_LEFT || region > UMI_WS_DOCK_FLOATING) return UMI_STATUS_INVALID_ARGUMENT;
    zone->region = region;
    zone->edge_fraction = umi_ws_clamp_unit(edge_fraction);
    return UMI_STATUS_OK;
}
