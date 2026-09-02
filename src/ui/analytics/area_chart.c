/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/area_chart.c
 *
 * PURPOSE:
 *   Configure area-chart stacking and fill opacity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/area_chart.h"

#include <string.h>
/*
 * Initialise analytics area chart from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_area_chart_init(UmiAnalyticsAreaChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->opacity=0.35;return UMI_STATUS_OK;}
/*
 * Check that analytics area chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_area_chart_valid(const UmiAnalyticsAreaChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->opacity>=0.0&&item->opacity<=1.0)?1:0;}
