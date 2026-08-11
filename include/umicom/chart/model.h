/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/model.h
 *
 * PURPOSE:
 *   Own the toolkit-neutral chart model and series catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Chart data is independent of GTK4, JavaScript and TradingView. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_MODEL_H
#define UMICOM_CHART_MODEL_H
#include "umicom/chart/series.h"
#include "umicom/chart/viewport.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiChartModel { char title[UMI_CHART_NAME_CAPACITY]; UmiChartSeries series[UMI_CHART_MAX_SERIES]; size_t series_count; UmiChartViewport viewport; uint64_t revision; } UmiChartModel;
UmiStatus umi_chart_model_init(UmiChartModel *model,const char *title);
UmiStatus umi_chart_model_add_series(UmiChartModel *model,const UmiChartSeries *series);
UmiChartSeries *umi_chart_model_find_series(UmiChartModel *model,const char *id);
#ifdef __cplusplus
}
#endif
#endif
