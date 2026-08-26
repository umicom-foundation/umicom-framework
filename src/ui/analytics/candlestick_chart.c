/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/candlestick_chart.c
 *
 * PURPOSE:
 *   Validate OHLC analytical samples for financial and operational range charts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/candlestick_chart.h"

#include <math.h>
UmiStatus umi_analytics_candle_init(UmiAnalyticsCandle *c,int64_t ts,double open,double high,double low,double close,double volume){if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;c->timestamp_ns=ts;c->open=open;c->high=high;c->low=low;c->close=close;c->volume=volume;return umi_analytics_candle_valid(c)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
int umi_analytics_candle_valid(const UmiAnalyticsCandle *c){if(c==NULL||!isfinite(c->open)||!isfinite(c->high)||!isfinite(c->low)||!isfinite(c->close)||!isfinite(c->volume)||c->volume<0.0)return 0;return c->high>=c->open&&c->high>=c->close&&c->low<=c->open&&c->low<=c->close&&c->high>=c->low;}
