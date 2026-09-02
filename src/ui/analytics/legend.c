/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/legend.c
 *
 * PURPOSE:
 *   Maintain bounded semantic legend entries and visibility state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/legend.h"

#include <string.h>
/*
 * Initialise analytics legend from caller-provided values so later operations receive a
 * known state.
 */
void umi_analytics_legend_init(UmiAnalyticsLegend *legend){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(legend!=NULL)memset(legend,0,sizeof *legend);}
/* Add analytics legend only after its inputs and available capacity have been checked. */
UmiStatus umi_analytics_legend_add(UmiAnalyticsLegend *legend,const char *series_id,const char *label){UmiAnalyticsLegendItem *item;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(legend==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(legend->count>=UMI_ANALYTICS_MAX_SERIES)return UMI_STATUS_CAPACITY_EXCEEDED;item=&legend->items[legend->count];s=umi_analytics_copy_text(item->series_id,sizeof item->series_id,series_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;s=umi_analytics_copy_text(item->label,sizeof item->label,label);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;item->visible=1;legend->count++;return UMI_STATUS_OK;}
/*
 * Provide the analytics legend set visible operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_legend_set_visible(UmiAnalyticsLegend *legend,const char *series_id,int visible){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(legend==NULL||series_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<legend->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(legend->items[i].series_id,series_id)==0){legend->items[i].visible=visible?1:0;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
