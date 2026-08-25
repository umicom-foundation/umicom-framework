/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/snap_policy.c
 *
 * PURPOSE:
 *   Implement deterministic snapping thresholds for panel edges, grids and designer canvases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/snap_policy.h"

UmiWsSnapPolicy umi_ws_snap_policy_default(UmiWsDensity density) {
    UmiWsSnapPolicy policy = {8, 8, true, true};
    if (density == UMI_WS_DENSITY_COMPACT) {
        policy.edge_threshold = 6;
        policy.grid_size = 6;
    } else if (density == UMI_WS_DENSITY_TOUCH) {
        policy.edge_threshold = 14;
        policy.grid_size = 12;
    }
    return policy;
}

int32_t umi_ws_snap_policy_coordinate(const UmiWsSnapPolicy *policy, int32_t coordinate) {
    int32_t grid;
    int32_t remainder;
    if (policy == NULL || !policy->snap_grid || policy->grid_size <= 1) return coordinate;
    grid = policy->grid_size;
    remainder = coordinate % grid;
    if (remainder < 0) remainder += grid;
    if (remainder * 2 >= grid) return coordinate + (grid - remainder);
    return coordinate - remainder;
}

bool umi_ws_snap_policy_near_edge(const UmiWsSnapPolicy *policy, int32_t coordinate, int32_t edge) {
    int32_t delta;
    if (policy == NULL || !policy->snap_edges) return false;
    delta = coordinate - edge;
    if (delta < 0) delta = -delta;
    return delta <= policy->edge_threshold;
}
