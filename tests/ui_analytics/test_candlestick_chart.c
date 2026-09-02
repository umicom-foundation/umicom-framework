/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_candlestick_chart.c
 *
 * PURPOSE:
 *   Validate candlestick_chart analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/candlestick_chart.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsCandle c;return umi_analytics_candle_init(&c,1,10,12,9,11,100)==0?0:1;}
