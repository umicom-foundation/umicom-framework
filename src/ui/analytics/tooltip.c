/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/tooltip.c
 *
 * PURPOSE:
 *   Format bounded analytical tooltip content without renderer markup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/tooltip.h"

#include <stdio.h>
#include <string.h>
/*
 * Copy analytics tooltip into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_analytics_tooltip_set(UmiAnalyticsTooltip *tooltip,const char *title,double x,double y){int written;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tooltip==NULL||!umi_analytics_number_valid(x)||!umi_analytics_number_valid(y))return UMI_STATUS_INVALID_ARGUMENT;memset(tooltip,0,sizeof *tooltip);s=umi_analytics_copy_text(tooltip->title,sizeof tooltip->title,title);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;written=snprintf(tooltip->text,sizeof tooltip->text,"x=%.6g, y=%.6g",x,y);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(written<0||(size_t)written>=sizeof tooltip->text)return UMI_STATUS_CAPACITY_EXCEEDED;tooltip->x=x;tooltip->y=y;tooltip->visible=1;return UMI_STATUS_OK;}
