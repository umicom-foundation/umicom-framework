/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/series.h
 *
 * PURPOSE:
 *   Manage bounded chart point series.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Chart data is independent of GTK4, JavaScript and TradingView. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_SERIES_H
#define UMICOM_CHART_SERIES_H
#include "umicom/chart/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiChartSeries { char id[UMI_CHART_NAME_CAPACITY]; UmiChartSeriesKind kind; UmiChartPoint points[UMI_CHART_MAX_POINTS]; size_t point_count; } UmiChartSeries;
UmiStatus umi_chart_series_init(UmiChartSeries *series,const char *id,UmiChartSeriesKind kind);
UmiStatus umi_chart_series_add(UmiChartSeries *series,UmiChartPoint point);
void umi_chart_series_clear(UmiChartSeries *series);
#ifdef __cplusplus
}
#endif
#endif
