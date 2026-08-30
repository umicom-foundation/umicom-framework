/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/snap_policy.h
 *
 * PURPOSE:
 *   Define deterministic snapping thresholds for panel edges, grids and designer canvases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_SNAP_POLICY_H
#define UMICOM_UI_WORKSTATION_SNAP_POLICY_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsSnapPolicy {
    int32_t edge_threshold;
    int32_t grid_size;
    bool snap_edges;
    bool snap_grid;
} UmiWsSnapPolicy;

UmiWsSnapPolicy umi_ws_snap_policy_default(UmiWsDensity density);
int32_t umi_ws_snap_policy_coordinate(const UmiWsSnapPolicy *policy, int32_t coordinate);
bool umi_ws_snap_policy_near_edge(const UmiWsSnapPolicy *policy, int32_t coordinate, int32_t edge);

#ifdef __cplusplus
}
#endif

#endif
