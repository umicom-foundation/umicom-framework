/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_filter.c
 *
 * PURPOSE:
 *   Describe one dashboard-level textual filter propagated to compatible tiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_filter.h"

#include <string.h>
UmiStatus umi_analytics_dashboard_filter_init(UmiAnalyticsDashboardFilter *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->key,sizeof item->key,"filter");(void)umi_analytics_copy_text(item->value,sizeof item->value,"all");return UMI_STATUS_OK;}
int umi_analytics_dashboard_filter_valid(const UmiAnalyticsDashboardFilter *item){if(item==NULL)return 0;return (item->key[0]!='\0')?1:0;}
