/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_snapshot.c
 *
 * PURPOSE:
 *   Represent immutable dashboard render/data snapshot metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_snapshot.h"

#include <string.h>
UmiStatus umi_analytics_dashboard_snapshot_init(UmiAnalyticsDashboardSnapshot *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->dashboard_id,sizeof item->dashboard_id,"dashboard");item->revision=1U;item->healthy=1;return UMI_STATUS_OK;}
int umi_analytics_dashboard_snapshot_valid(const UmiAnalyticsDashboardSnapshot *item){if(item==NULL)return 0;return (item->dashboard_id[0]!='\0'&&item->revision>0U)?1:0;}
