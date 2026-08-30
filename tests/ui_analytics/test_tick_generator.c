/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_tick_generator.c
 *
 * PURPOSE:
 *   Validate tick_generator analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/tick_generator.h"

int main(void){UmiAnalyticsTicks t;if(umi_analytics_ticks_linear(0,10,3,&t)!=0)return 1;return t.values[1]==5.0?0:2;}
