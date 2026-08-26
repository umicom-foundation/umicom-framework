/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_candlestick_chart.c
 *
 * PURPOSE:
 *   Validate candlestick_chart analytics behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/candlestick_chart.h"

int main(void){UmiAnalyticsCandle c;return umi_analytics_candle_init(&c,1,10,12,9,11,100)==0?0:1;}
