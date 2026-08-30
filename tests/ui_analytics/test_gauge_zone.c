/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_gauge_zone.c
 *
 * PURPOSE:
 *   Validate gauge_zone analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/gauge_zone.h"

int main(void){UmiAnalyticsGaugeZone item;if(umi_analytics_gauge_zone_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_gauge_zone_valid(&item))?0:2;}
