/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/area_chart.c
 *
 * PURPOSE:
 *   Configure area-chart stacking and fill opacity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/area_chart.h"

#include <string.h>
UmiStatus umi_analytics_area_chart_init(UmiAnalyticsAreaChart *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->opacity=0.35;return UMI_STATUS_OK;}
int umi_analytics_area_chart_valid(const UmiAnalyticsAreaChart *item){if(item==NULL)return 0;return (item->opacity>=0.0&&item->opacity<=1.0)?1:0;}
