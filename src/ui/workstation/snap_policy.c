/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/snap_policy.c
 *
 * PURPOSE:
 *   Implement deterministic snapping thresholds for panel edges, grids and designer canvases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/snap_policy.h"

/*
 * Provide the ws snap policy default operation used by this module and its client
 * applications.
 */
UmiWsSnapPolicy umi_ws_snap_policy_default(UmiWsDensity density) {
    UmiWsSnapPolicy policy = {8, 8, true, true};
    /* Apply this branch only when its contract condition is satisfied. */
    if (density == UMI_WS_DENSITY_COMPACT) {
        policy.edge_threshold = 6;
        policy.grid_size = 6;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (density == UMI_WS_DENSITY_TOUCH) {
        policy.edge_threshold = 14;
        policy.grid_size = 12;
    }
    return policy;
}

/*
 * Provide the ws snap policy coordinate operation used by this module and its client
 * applications.
 */
int32_t umi_ws_snap_policy_coordinate(const UmiWsSnapPolicy *policy, int32_t coordinate) {
    int32_t grid;
    int32_t remainder;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !policy->snap_grid || policy->grid_size <= 1) return coordinate;
    grid = policy->grid_size;
    remainder = coordinate % grid;
    /* Apply this branch only when its contract condition is satisfied. */
    if (remainder < 0) remainder += grid;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (remainder * 2 >= grid) return coordinate + (grid - remainder);
    return coordinate - remainder;
}

/*
 * Provide the ws snap policy near edge operation used by this module and its client
 * applications.
 */
bool umi_ws_snap_policy_near_edge(const UmiWsSnapPolicy *policy, int32_t coordinate, int32_t edge) {
    int32_t delta;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !policy->snap_edges) return false;
    delta = coordinate - edge;
    /* Apply this branch only when its contract condition is satisfied. */
    if (delta < 0) delta = -delta;
    return delta <= policy->edge_threshold;
}
