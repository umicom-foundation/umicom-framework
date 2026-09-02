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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_CROSSHAIR_H
#define UMICOM_UI_ANALYTICS_CROSSHAIR_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics crosshair data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsCrosshair { double x; double y; int visible; int locked; } UmiAnalyticsCrosshair;
/**
 * Initialise analytics crosshair from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_crosshair_init(UmiAnalyticsCrosshair *item);
/**
 * Check that analytics crosshair satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_crosshair_valid(const UmiAnalyticsCrosshair *item);

#ifdef __cplusplus
}
#endif

#endif
