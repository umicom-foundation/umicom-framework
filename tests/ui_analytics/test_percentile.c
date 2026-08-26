/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_percentile.c
 *
 * PURPOSE:
 *   Validate percentile analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/percentile.h"

int main(void){double v[]={0,10,20,30},p;if(umi_analytics_percentile(v,4,50,&p)!=0)return 1;return p==15.0?0:2;}
