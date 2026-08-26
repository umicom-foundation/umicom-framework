/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_min_max.c
 *
 * PURPOSE:
 *   Validate min_max analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/min_max.h"

int main(void){double v[]={3,1,9};UmiAnalyticsMinMax r;if(umi_analytics_min_max_compute(v,3,&r)!=0)return 1;return r.minimum==1.0&&r.maximum==9.0?0:2;}
