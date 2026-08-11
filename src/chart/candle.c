/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/candle.c
 *
 * PURPOSE:
 *   Validate OHLCV candles and expose simple candle measurements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The calculation is deliberately straightforward and deterministic. Frontends can optimise rendering without changing these shared data contracts.
 */

#include "umicom/chart/candle.h"
UmiStatus umi_chart_candle_validate(const UmiChartCandle *c){if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(c->high<c->low||c->high<c->open||c->high<c->close||c->low>c->open||c->low>c->close||c->volume<0.0)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
double umi_chart_candle_range(const UmiChartCandle *c){return c!=NULL?c->high-c->low:0.0;}
int umi_chart_candle_is_up(const UmiChartCandle *c){return c!=NULL&&c->close>=c->open;}
