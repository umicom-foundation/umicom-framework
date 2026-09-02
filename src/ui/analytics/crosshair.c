/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/crosshair.c
 *
 * PURPOSE:
 *   Track semantic crosshair position, visibility and lock state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/crosshair.h"

#include <string.h>
/*
 * Initialise analytics crosshair from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_crosshair_init(UmiAnalyticsCrosshair *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->x=0.0;item->y=0.0;return UMI_STATUS_OK;}
/*
 * Check that analytics crosshair satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_crosshair_valid(const UmiAnalyticsCrosshair *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (umi_analytics_number_valid(item->x)&&umi_analytics_number_valid(item->y))?1:0;}
