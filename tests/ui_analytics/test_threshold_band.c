/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_threshold_band.c
 *
 * PURPOSE:
 *   Validate threshold_band analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/threshold_band.h"

int main(void){UmiAnalyticsThresholdBand item;if(umi_analytics_threshold_band_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_threshold_band_valid(&item))?0:2;}
