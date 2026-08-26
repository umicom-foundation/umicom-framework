/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/crosshair.h
 *
 * PURPOSE:
 *   Track semantic crosshair position, visibility and lock state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_CROSSHAIR_H
#define UMICOM_UI_ANALYTICS_CROSSHAIR_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsCrosshair { double x; double y; int visible; int locked; } UmiAnalyticsCrosshair;
UmiStatus umi_analytics_crosshair_init(UmiAnalyticsCrosshair *item);
int umi_analytics_crosshair_valid(const UmiAnalyticsCrosshair *item);

#ifdef __cplusplus
}
#endif

#endif
