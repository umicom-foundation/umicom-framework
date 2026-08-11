/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/snapshot.h
 *
 * PURPOSE:
 *   Expose a compact immutable chart-state snapshot.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Chart data is independent of GTK4, JavaScript and TradingView. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_SNAPSHOT_H
#define UMICOM_CHART_SNAPSHOT_H
#include "umicom/chart/model.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiChartSnapshot { char title[UMI_CHART_NAME_CAPACITY]; size_t series_count; size_t point_count; uint64_t revision; } UmiChartSnapshot;
UmiStatus umi_chart_snapshot(const UmiChartModel *model,UmiChartSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
