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

/*
 * Chart data is independent of any frontend toolkit or renderer. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_SERIES_H
#define UMICOM_CHART_SERIES_H
#include "umicom/chart/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the chart series data shared with callers of this public contract.
 */
typedef struct UmiChartSeries { char id[UMI_CHART_NAME_CAPACITY]; UmiChartSeriesKind kind; UmiChartPoint points[UMI_CHART_MAX_POINTS]; size_t point_count; } UmiChartSeries;
/**
 * Initialise chart series from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_chart_series_init(UmiChartSeries *series,const char *id,UmiChartSeriesKind kind);
/**
 * Add chart series only after its inputs and available capacity have been checked.
 */
UmiStatus umi_chart_series_add(UmiChartSeries *series,UmiChartPoint point);
/**
 * Release or reset state held by chart series so the same storage can be reused safely.
 */
void umi_chart_series_clear(UmiChartSeries *series);
#ifdef __cplusplus
}
#endif
#endif
