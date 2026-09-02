/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_heatmap_cell.c
 *
 * PURPOSE:
 *   Validate heatmap_cell analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_cell.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsHeatmapCell item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_heatmap_cell_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_heatmap_cell_valid(&item))?0:2;}
