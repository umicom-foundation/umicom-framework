/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/market_bar_private.h
 * PURPOSE: Share decimal and OHLC validation between historical and streaming messages.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_MARKET_BAR_PRIVATE_H
#define UMICOM_IBKR_MARKET_BAR_PRIVATE_H
#include "umicom/broker_connectivity/historical_bars.h"
/* The caller validates the surrounding message and supplies eight fields.
 * Decode into a temporary record; publish it only after this function succeeds. */
bool UmiIbkrMarketBarRead(char **fields, UmiIbkrHistoricalDataKind kind, UmiIbkrHistoricalBar *outBar);
#endif
