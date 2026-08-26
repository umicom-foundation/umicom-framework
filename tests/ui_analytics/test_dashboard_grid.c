/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_grid.c
 *
 * PURPOSE:
 *   Validate dashboard_grid analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_grid.h"

int main(void){UmiAnalyticsDashboardGrid g;UmiAnalyticsDashboardTile t;if(umi_analytics_dashboard_grid_init(&g,4,4)!=0)return 1;if(umi_analytics_dashboard_tile_init(&t)!=0)return 2;t.row_span=2;t.column_span=2;return umi_analytics_dashboard_grid_add(&g,&t)==0?0:3;}
