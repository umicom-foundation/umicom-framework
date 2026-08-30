/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/heatmap_cell.c
 *
 * PURPOSE:
 *   Describe one labelled heatmap cell for tooltip and accessibility projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_cell.h"

#include <string.h>
UmiStatus umi_analytics_heatmap_cell_init(UmiAnalyticsHeatmapCell *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->label,sizeof item->label,"Cell");return UMI_STATUS_OK;}
int umi_analytics_heatmap_cell_valid(const UmiAnalyticsHeatmapCell *item){if(item==NULL)return 0;return (umi_analytics_number_valid(item->value))?1:0;}
