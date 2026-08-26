/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/line_chart.c
 *
 * PURPOSE:
 *   Configure line-chart interpolation and marker semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/line_chart.h"

#include <string.h>
UmiStatus umi_analytics_line_chart_init(UmiAnalyticsLineChart *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->stroke_width=1.0;return UMI_STATUS_OK;}
int umi_analytics_line_chart_valid(const UmiAnalyticsLineChart *item){if(item==NULL)return 0;return (item->stroke_width>0.0)?1:0;}
