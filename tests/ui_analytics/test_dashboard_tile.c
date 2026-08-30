/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_tile.c
 *
 * PURPOSE:
 *   Validate dashboard_tile analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_tile.h"

int main(void){UmiAnalyticsDashboardTile item;if(umi_analytics_dashboard_tile_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_dashboard_tile_valid(&item))?0:2;}
