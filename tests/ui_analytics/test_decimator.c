/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_decimator.c
 *
 * PURPOSE:
 *   Validate decimator analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/decimator.h"

int main(void){double v[]={1,9,3,4};UmiAnalyticsDecimatedPair p[2];if(umi_analytics_decimate_min_max(v,4,2,p,2)!=0)return 1;return p[0].minimum==1&&p[0].maximum==9?0:2;}
