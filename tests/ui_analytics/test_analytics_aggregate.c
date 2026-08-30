/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_aggregate.c
 *
 * PURPOSE:
 *   Validate analytics_aggregate analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_aggregate.h"

int main(void){double v[]={1,2,6},a;if(umi_analytics_aggregate_compute(UMI_ANALYTICS_AGGREGATE_AVERAGE,v,3,&a)!=0)return 1;return a==3.0?0:2;}
