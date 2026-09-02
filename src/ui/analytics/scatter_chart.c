/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/scatter_chart.c
 *
 * PURPOSE:
 *   Configure scatter point radius and optional trend presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/scatter_chart.h"

#include <string.h>
/*
 * Initialise analytics scatter chart from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_scatter_chart_init(UmiAnalyticsScatterChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->point_radius=3.0;return UMI_STATUS_OK;}
/*
 * Check that analytics scatter chart satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_scatter_chart_valid(const UmiAnalyticsScatterChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->point_radius>0.0)?1:0;}
