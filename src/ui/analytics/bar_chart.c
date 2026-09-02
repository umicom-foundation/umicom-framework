/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/bar_chart.c
 *
 * PURPOSE:
 *   Configure grouped bar-chart orientation and relative gap.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/bar_chart.h"

#include <string.h>
/*
 * Initialise analytics bar chart from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_bar_chart_init(UmiAnalyticsBarChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->orientation=UMI_ANALYTICS_VERTICAL;item->gap_ratio=0.2;return UMI_STATUS_OK;}
/*
 * Check that analytics bar chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_bar_chart_valid(const UmiAnalyticsBarChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->orientation>=UMI_ANALYTICS_HORIZONTAL&&item->orientation<=UMI_ANALYTICS_VERTICAL&&item->gap_ratio>=0.0&&item->gap_ratio<1.0)?1:0;}
