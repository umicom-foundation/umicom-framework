/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/dock_zone.h
 *
 * PURPOSE:
 *   Define screen-relative docking zone classification used by mouse and keyboard layout operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DOCK_ZONE_H
#define UMICOM_UI_WORKSTATION_DOCK_ZONE_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws dock zone data shared with callers of this public contract.
 */
typedef struct UmiWsDockZone {
    UmiWsDockRegion region;
    double edge_fraction;
} UmiWsDockZone;

/**
 * Provide the ws dock zone classify operation used by this module and its client
 * applications.
 */
UmiWsDockRegion umi_ws_dock_zone_classify(double x_ratio, double y_ratio, double edge_fraction);
/**
 * Initialise ws dock zone from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ws_dock_zone_init(UmiWsDockZone *zone, UmiWsDockRegion region, double edge_fraction);

#ifdef __cplusplus
}
#endif

#endif
