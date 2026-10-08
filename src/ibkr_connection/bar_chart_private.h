/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/bar_chart_private.h
 * PURPOSE: Share chart projection without making broker transport depend on graphics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_BAR_CHART_PRIVATE_H
#define UMICOM_IBKR_BAR_CHART_PRIVATE_H
#include "umicom/broker_connectivity/historical_chart.h"
UmiStatus UmiIbkrMarketBarCandle(const UmiIbkrHistoricalBar *, UmiChartCandle *, bool *);
UmiStatus UmiIbkrMarketBarScene(const UmiChartCandle *, size_t, const char *, UmiChartRenderScene **);
#endif
