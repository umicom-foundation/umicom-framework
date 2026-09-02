/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_layout.c
 *
 * PURPOSE:
 *   Describe named responsive dashboard layout metadata around a canonical grid.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_layout.h"

#include <string.h>
/*
 * Initialise analytics dashboard layout from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_layout_init(UmiAnalyticsDashboardLayout *layout,const char *id,const char *title,uint16_t rows,uint16_t columns){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(layout==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(layout,0,sizeof *layout);s=umi_analytics_copy_text(layout->id,sizeof layout->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;s=umi_analytics_copy_text(layout->title,sizeof layout->title,title);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;return umi_analytics_dashboard_grid_init(&layout->grid,rows,columns);}
