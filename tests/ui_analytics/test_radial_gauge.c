/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_radial_gauge.c
 *
 * PURPOSE:
 *   Validate radial_gauge analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/radial_gauge.h"

int main(void){UmiAnalyticsRadialGauge item;if(umi_analytics_radial_gauge_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_radial_gauge_valid(&item))?0:2;}
