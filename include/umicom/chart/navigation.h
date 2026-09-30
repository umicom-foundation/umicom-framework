/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/navigation.h
 * PURPOSE: Keep chart navigation anchored to retained time-series data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_NAVIGATION_H
#define UMICOM_CHART_NAVIGATION_H
#include "umicom/chart/plot.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Zero initialization follows the newest candle and fits all retained bars.
 * A pinned timestamp keeps a historical view stable when new bars arrive.
 * Navigation does not own or alter prices. Access on the UI owner's thread. */
typedef struct UmiChartNavigation {
    size_t visible_bars;
    int64_t anchor_ms;
    int pinned;
} UmiChartNavigation;
typedef struct UmiChartWindow { size_t first; size_t count; } UmiChartWindow;
UmiStatus UmiChartNavigationResolve(const UmiChartNavigation *navigation,
    const UmiChartCandle *candles, size_t count, UmiChartWindow *out_window);
/* Positive zoom steps show fewer bars; negative steps show more. Pan steps
 * move by bars. Both clamp to retained history; no unseen data is invented. */
UmiStatus UmiChartNavigationZoom(UmiChartNavigation *navigation,
    const UmiChartCandle *candles, size_t count, int steps);
UmiStatus UmiChartNavigationPan(UmiChartNavigation *navigation,
    const UmiChartCandle *candles, size_t count, int bars);
/* Convert logical scene coordinates to a data anchor. Outside-plot points,
 * non-finite geometry and invalid ranges fail without changing output. */
UmiStatus UmiChartPlotUnmap(const UmiChartPlotViewport *viewport,
    double x, double y, UmiChartPoint *out_point);
#ifdef __cplusplus
}
#endif
#endif
