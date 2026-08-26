/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_tooltip.c
 *
 * PURPOSE:
 *   Validate tooltip analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/tooltip.h"

#include <string.h>
int main(void){UmiAnalyticsTooltip t;if(umi_analytics_tooltip_set(&t,"Point",1,2)!=0)return 1;return strstr(t.text,"y=2")!=NULL?0:2;}
