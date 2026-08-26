/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/tooltip.c
 *
 * PURPOSE:
 *   Format bounded analytical tooltip content without renderer markup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/tooltip.h"

#include <stdio.h>
#include <string.h>
UmiStatus umi_analytics_tooltip_set(UmiAnalyticsTooltip *tooltip,const char *title,double x,double y){int written;UmiStatus s;if(tooltip==NULL||!umi_analytics_number_valid(x)||!umi_analytics_number_valid(y))return UMI_STATUS_INVALID_ARGUMENT;memset(tooltip,0,sizeof *tooltip);s=umi_analytics_copy_text(tooltip->title,sizeof tooltip->title,title);if(s!=0)return s;written=snprintf(tooltip->text,sizeof tooltip->text,"x=%.6g, y=%.6g",x,y);if(written<0||(size_t)written>=sizeof tooltip->text)return UMI_STATUS_CAPACITY_EXCEEDED;tooltip->x=x;tooltip->y=y;tooltip->visible=1;return UMI_STATUS_OK;}
