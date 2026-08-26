/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_crosshair.c
 *
 * PURPOSE:
 *   Validate crosshair analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/crosshair.h"

int main(void){UmiAnalyticsCrosshair item;if(umi_analytics_crosshair_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_crosshair_valid(&item))?0:2;}
