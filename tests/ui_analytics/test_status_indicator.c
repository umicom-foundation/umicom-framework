/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_status_indicator.c
 *
 * PURPOSE:
 *   Validate status_indicator analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/status_indicator.h"

int main(void){UmiAnalyticsStatusIndicator item;if(umi_analytics_status_indicator_init(&item)!=UMI_STATUS_OK)return 1;return (umi_analytics_status_indicator_valid(&item))?0:2;}
