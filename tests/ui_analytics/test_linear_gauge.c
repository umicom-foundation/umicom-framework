/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_linear_gauge.c
 *
 * PURPOSE:
 *   Validate linear_gauge analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/linear_gauge.h"

int main(void){UmiAnalyticsLinearGauge item;if(umi_analytics_linear_gauge_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_linear_gauge_valid(&item))?0:2;}
