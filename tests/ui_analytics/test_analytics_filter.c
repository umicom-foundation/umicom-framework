/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_filter.c
 *
 * PURPOSE:
 *   Validate analytics_filter analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_filter.h"

int main(void){UmiAnalyticsFilter f;if(umi_analytics_filter_init(&f,UMI_ANALYTICS_COMPARE_GTE,10)!=0)return 1;return umi_analytics_filter_matches(&f,11)?0:2;}
