/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/chart_model.c
 *
 * PURPOSE:
 *   Compose canonical Design System chart specifications with axes and series references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/chart_model.h"

#include <string.h>
/*
 * Initialise analytics chart model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_chart_model_init(UmiAnalyticsChartModel *chart,const char *id,const char *title,UmiDesignChartKind kind){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chart==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(chart,0,sizeof *chart);s=umi_analytics_copy_text(chart->id,sizeof chart->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;s=umi_analytics_copy_text(chart->title,sizeof chart->title,title);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;chart->spec.kind=kind;chart->spec.series_count=0U;chart->spec.legend=1;chart->spec.crosshair=1;chart->spec.zoom=1;chart->spec.pan=1;umi_analytics_series_registry_init(&chart->series);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_analytics_axis_init(&chart->x_axis,"x","X",UMI_ANALYTICS_SCALE_LINEAR,0.0,1.0)!=0)return UMI_STATUS_INTERNAL_ERROR;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_analytics_axis_init(&chart->y_axis,"y","Y",UMI_ANALYTICS_SCALE_LINEAR,0.0,1.0)!=0)return UMI_STATUS_INTERNAL_ERROR;return UMI_STATUS_OK;}
/*
 * Provide the analytics chart model add series operation used by this module and its
 * client applications.
 */
UmiStatus umi_analytics_chart_model_add_series(UmiAnalyticsChartModel *chart,UmiAnalyticsSeries *series){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(chart==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_analytics_series_registry_add(&chart->series,series);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==0){chart->spec.series_count=(uint16_t)chart->series.count;chart->revision++;}return s;}
