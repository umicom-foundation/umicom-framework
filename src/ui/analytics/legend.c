/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/legend.c
 *
 * PURPOSE:
 *   Maintain bounded semantic legend entries and visibility state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/legend.h"

#include <string.h>
void umi_analytics_legend_init(UmiAnalyticsLegend *legend){if(legend!=NULL)memset(legend,0,sizeof *legend);}
UmiStatus umi_analytics_legend_add(UmiAnalyticsLegend *legend,const char *series_id,const char *label){UmiAnalyticsLegendItem *item;UmiStatus s;if(legend==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(legend->count>=UMI_ANALYTICS_MAX_SERIES)return UMI_STATUS_CAPACITY_EXCEEDED;item=&legend->items[legend->count];s=umi_analytics_copy_text(item->series_id,sizeof item->series_id,series_id);if(s!=0)return s;s=umi_analytics_copy_text(item->label,sizeof item->label,label);if(s!=0)return s;item->visible=1;legend->count++;return UMI_STATUS_OK;}
UmiStatus umi_analytics_legend_set_visible(UmiAnalyticsLegend *legend,const char *series_id,int visible){size_t i;if(legend==NULL||series_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<legend->count;i++)if(strcmp(legend->items[i].series_id,series_id)==0){legend->items[i].visible=visible?1:0;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
