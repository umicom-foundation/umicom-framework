/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_tile.c
 *
 * PURPOSE:
 *   Describe one grid-positioned dashboard tile and its semantic component identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_tile.h"

#include <string.h>
/*
 * Initialise analytics dashboard tile from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_tile_init(UmiAnalyticsDashboardTile *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->id,sizeof item->id,"tile");(void)umi_analytics_copy_text(item->component_id,sizeof item->component_id,"component");item->row_span=1U;item->column_span=1U;return UMI_STATUS_OK;}
/*
 * Check that analytics dashboard tile satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_dashboard_tile_valid(const UmiAnalyticsDashboardTile *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->id[0]!='\0'&&item->component_id[0]!='\0'&&item->row_span>0U&&item->column_span>0U)?1:0;}
