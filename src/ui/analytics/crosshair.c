/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/crosshair.c
 *
 * PURPOSE:
 *   Track semantic crosshair position, visibility and lock state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/crosshair.h"

#include <string.h>
UmiStatus umi_analytics_crosshair_init(UmiAnalyticsCrosshair *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->x=0.0;item->y=0.0;return UMI_STATUS_OK;}
int umi_analytics_crosshair_valid(const UmiAnalyticsCrosshair *item){if(item==NULL)return 0;return (umi_analytics_number_valid(item->x)&&umi_analytics_number_valid(item->y))?1:0;}
