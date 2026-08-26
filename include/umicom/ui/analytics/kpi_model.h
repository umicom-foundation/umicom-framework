/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/kpi_model.h
 *
 * PURPOSE:
 *   Evaluate KPI progress against baseline and target values.
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
#ifndef UMICOM_UI_ANALYTICS_KPI_MODEL_H
#define UMICOM_UI_ANALYTICS_KPI_MODEL_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsKpiModel { double baseline; double target; double current; int higher_is_better; } UmiAnalyticsKpiModel;
UmiStatus umi_analytics_kpi_model_init(UmiAnalyticsKpiModel *k,double baseline,double target,double current,int higher_is_better);
UmiStatus umi_analytics_kpi_model_progress(const UmiAnalyticsKpiModel *k,double *out_progress);

#ifdef __cplusplus
}
#endif

#endif
