/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/types.h
 *
 * PURPOSE:
 *   Define toolkit-neutral financial-chart primitives.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Chart data is independent of GTK4, JavaScript and TradingView. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_TYPES_H
#define UMICOM_CHART_TYPES_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CHART_NAME_CAPACITY 128U
#define UMI_CHART_MAX_POINTS 4096U
#define UMI_CHART_MAX_SERIES 32U
typedef enum UmiChartSeriesKind { UMI_CHART_LINE=1, UMI_CHART_CANDLE=2, UMI_CHART_HISTOGRAM=3 } UmiChartSeriesKind;
typedef struct UmiChartPoint { int64_t time_ms; double value; } UmiChartPoint;
typedef struct UmiChartCandle { int64_t time_ms; double open,high,low,close,volume; } UmiChartCandle;
#ifdef __cplusplus
}
#endif
#endif
