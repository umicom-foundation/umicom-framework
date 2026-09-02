/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/candle.c
 *
 * PURPOSE:
 *   Validate OHLCV candles and expose simple candle measurements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The calculation is deliberately straightforward and deterministic. Frontends can optimise rendering without changing these shared data contracts.
 */

#include "umicom/chart/candle.h"
/* Check that chart candle satisfies its contract before another service relies on it. */
UmiStatus umi_chart_candle_validate(const UmiChartCandle *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->high<c->low||c->high<c->open||c->high<c->close||c->low>c->open||c->low>c->close||c->volume<0.0)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Provide the chart candle range operation used by this module and its client
 * applications.
 */
double umi_chart_candle_range(const UmiChartCandle *c){return c!=NULL?c->high-c->low:0.0;}
/*
 * Provide the chart candle is up operation used by this module and its client
 * applications.
 */
int umi_chart_candle_is_up(const UmiChartCandle *c){return c!=NULL&&c->close>=c->open;}
