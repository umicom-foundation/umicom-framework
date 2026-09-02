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
/*
 * Initialise analytics dashboard snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_snapshot_init(UmiAnalyticsDashboardSnapshot *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->dashboard_id,sizeof item->dashboard_id,"dashboard");item->revision=1U;item->healthy=1;return UMI_STATUS_OK;}
/*
 * Check that analytics dashboard snapshot satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_dashboard_snapshot_valid(const UmiAnalyticsDashboardSnapshot *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->dashboard_id[0]!='\0'&&item->revision>0U)?1:0;}
