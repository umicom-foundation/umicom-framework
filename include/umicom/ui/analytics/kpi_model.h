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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_KPI_MODEL_H
#define UMICOM_UI_ANALYTICS_KPI_MODEL_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics kpi model data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsKpiModel { double baseline; double target; double current; int higher_is_better; } UmiAnalyticsKpiModel;
/**
 * Initialise analytics kpi model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_kpi_model_init(UmiAnalyticsKpiModel *k,double baseline,double target,double current,int higher_is_better);
/**
 * Provide the analytics kpi model progress operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_kpi_model_progress(const UmiAnalyticsKpiModel *k,double *out_progress);

#ifdef __cplusplus
}
#endif

#endif
