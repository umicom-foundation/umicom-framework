/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_layout.c
 *
 * PURPOSE:
 *   Describe named responsive dashboard layout metadata around a canonical grid.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_layout.h"

#include <string.h>
UmiStatus umi_analytics_dashboard_layout_init(UmiAnalyticsDashboardLayout *layout,const char *id,const char *title,uint16_t rows,uint16_t columns){UmiStatus s;if(layout==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(layout,0,sizeof *layout);s=umi_analytics_copy_text(layout->id,sizeof layout->id,id);if(s!=0)return s;s=umi_analytics_copy_text(layout->title,sizeof layout->title,title);if(s!=0)return s;return umi_analytics_dashboard_grid_init(&layout->grid,rows,columns);}
