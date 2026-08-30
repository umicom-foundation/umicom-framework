/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_trading_common.h
 *
 * PURPOSE:
 *   Provide deterministic instrument and order fixtures shared by trading foundation tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Tests use fixed values so a failure can be reproduced exactly and is not dependent on live markets.
 */

#ifndef UMICOM_TEST_TRADING_COMMON_H
#define UMICOM_TEST_TRADING_COMMON_H
#include <stdio.h>
#include <string.h>
#include "umicom/trading/trading.h"

static inline UmiInstrument test_instrument(void)
{
    UmiInstrument instrument = {0};
    (void)snprintf(instrument.instrument_id.value, sizeof(instrument.instrument_id.value), "%s", "CME.NQ.202609");
    (void)snprintf(instrument.symbol, sizeof(instrument.symbol), "%s", "NQ");
    (void)snprintf(instrument.venue, sizeof(instrument.venue), "%s", "CME");
    (void)snprintf(instrument.currency.code, sizeof(instrument.currency.code), "%s", "USD");
    instrument.multiplier = 20.0;
    instrument.expiry_yyyymmdd = 20260918;
    return instrument;
}

static inline UmiOrderRequest test_order_request(void)
{
    UmiOrderRequest request = {0};
    (void)snprintf(request.client_order_id.value, sizeof(request.client_order_id.value), "%s", "ORDER-1");
    (void)snprintf(request.account_id.value, sizeof(request.account_id.value), "%s", "ACCOUNT-1");
    request.instrument = test_instrument();
    request.side = UMI_SIDE_BUY;
    request.type = UMI_ORDER_LIMIT;
    request.tif = UMI_TIF_DAY;
    request.quantity = 1.0;
    request.limit_price = 25000.0;
    request.environment = UMI_TRADING_SIMULATION;
    return request;
}
#endif
