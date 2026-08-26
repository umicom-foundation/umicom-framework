/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_rolling_statistics.c
 *
 * PURPOSE:
 *   Validate rolling_statistics analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/rolling_statistics.h"

int main(void){double v[]={1,3,5},o[3];if(umi_analytics_rolling_mean(v,3,2,o,3)!=0)return 1;return o[2]==4.0?0:2;}
