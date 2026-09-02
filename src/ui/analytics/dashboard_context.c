/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_context.c
 *
 * PURPOSE:
 *   Carry linked entity and time-window context across dashboard tiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_context.h"

#include <string.h>
/*
 * Initialise analytics dashboard context from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_context_init(UmiAnalyticsDashboardContext *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->context_group,sizeof item->context_group,"default");return UMI_STATUS_OK;}
/*
 * Check that analytics dashboard context satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_dashboard_context_valid(const UmiAnalyticsDashboardContext *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->context_group[0]!='\0'&&item->start_ns<=item->end_ns)?1:0;}
