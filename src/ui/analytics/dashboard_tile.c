/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_tile.c
 *
 * PURPOSE:
 *   Describe one grid-positioned dashboard tile and its semantic component identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_tile.h"

#include <string.h>
UmiStatus umi_analytics_dashboard_tile_init(UmiAnalyticsDashboardTile *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->id,sizeof item->id,"tile");(void)umi_analytics_copy_text(item->component_id,sizeof item->component_id,"component");item->row_span=1U;item->column_span=1U;return UMI_STATUS_OK;}
int umi_analytics_dashboard_tile_valid(const UmiAnalyticsDashboardTile *item){if(item==NULL)return 0;return (item->id[0]!='\0'&&item->component_id[0]!='\0'&&item->row_span>0U&&item->column_span>0U)?1:0;}
