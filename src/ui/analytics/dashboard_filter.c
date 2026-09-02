/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_filter.c
 *
 * PURPOSE:
 *   Describe one dashboard-level textual filter propagated to compatible tiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_filter.h"

#include <string.h>
/*
 * Initialise analytics dashboard filter from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_filter_init(UmiAnalyticsDashboardFilter *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->key,sizeof item->key,"filter");(void)umi_analytics_copy_text(item->value,sizeof item->value,"all");return UMI_STATUS_OK;}
/*
 * Check that analytics dashboard filter satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_dashboard_filter_valid(const UmiAnalyticsDashboardFilter *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->key[0]!='\0')?1:0;}
