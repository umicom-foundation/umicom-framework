/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/chart_annotation.c
 *
 * PURPOSE:
 *   Describe semantic chart annotations independent of renderer markup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/chart_annotation.h"

#include <string.h>
UmiStatus umi_analytics_chart_annotation_init(UmiAnalyticsChartAnnotation *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->id,sizeof item->id,"annotation");(void)umi_analytics_copy_text(item->text,sizeof item->text,"Annotation");return UMI_STATUS_OK;}
int umi_analytics_chart_annotation_valid(const UmiAnalyticsChartAnnotation *item){if(item==NULL)return 0;return (item->id[0]!='\0'&&umi_analytics_number_valid(item->x)&&umi_analytics_number_valid(item->y))?1:0;}
