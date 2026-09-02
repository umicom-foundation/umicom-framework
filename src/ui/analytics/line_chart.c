/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/line_chart.c
 *
 * PURPOSE:
 *   Configure line-chart interpolation and marker semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/line_chart.h"

#include <string.h>
/*
 * Initialise analytics line chart from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_line_chart_init(UmiAnalyticsLineChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->stroke_width=1.0;return UMI_STATUS_OK;}
/*
 * Check that analytics line chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_line_chart_valid(const UmiAnalyticsLineChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->stroke_width>0.0)?1:0;}
