/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/dock_zone.h
 *
 * PURPOSE:
 *   Define screen-relative docking zone classification used by mouse and keyboard layout operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DOCK_ZONE_H
#define UMICOM_UI_WORKSTATION_DOCK_ZONE_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsDockZone {
    UmiWsDockRegion region;
    double edge_fraction;
} UmiWsDockZone;

UmiWsDockRegion umi_ws_dock_zone_classify(double x_ratio, double y_ratio, double edge_fraction);
UmiStatus umi_ws_dock_zone_init(UmiWsDockZone *zone, UmiWsDockRegion region, double edge_fraction);

#ifdef __cplusplus
}
#endif

#endif
