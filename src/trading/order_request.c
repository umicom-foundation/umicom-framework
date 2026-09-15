/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_request.c
 *
 * PURPOSE:
 *   Validate canonical order requests before risk evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of order request. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_request.h"
#include <math.h>
#include "umicom/finance/identifier.h"
#include "umicom/trading/instrument.h"

/* Check that order request satisfies its contract before another service relies on it. */
UmiStatus umi_order_request_validate(const UmiOrderRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL ||
        !umi_financial_id_valid(&request->client_order_id) ||
        !umi_financial_id_valid(&request->account_id) ||
        !umi_instrument_valid(&request->instrument) ||
        !isfinite(request->quantity) || request->quantity <= 0.0 ||
        !isfinite(request->limit_price) || !isfinite(request->stop_price)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Validate explicit declared values, not enum ranges or truthiness: SELL
     * is -1. This recognises the environment value only; it grants no trading
     * permission, authentication, approval or live-order arming authority.
     */
    if ((request->side != UMI_SIDE_BUY && request->side != UMI_SIDE_SELL) ||
        (request->type != UMI_ORDER_MARKET && request->type != UMI_ORDER_LIMIT &&
         request->type != UMI_ORDER_STOP && request->type != UMI_ORDER_STOP_LIMIT) ||
        (request->tif != UMI_TIF_DAY && request->tif != UMI_TIF_GTC &&
         request->tif != UMI_TIF_IOC && request->tif != UMI_TIF_FOK) ||
        (request->environment != UMI_TRADING_SIMULATION &&
         request->environment != UMI_TRADING_PAPER &&
         request->environment != UMI_TRADING_LIVE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Both price fields must be finite even when inactive. Preserve the
     * established positivity requirements below: limit and stop prices are
     * required only for the order types that use them.
     */
    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->type == UMI_ORDER_LIMIT ||
         request->type == UMI_ORDER_STOP_LIMIT) &&
        request->limit_price <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((request->type == UMI_ORDER_STOP ||
         request->type == UMI_ORDER_STOP_LIMIT) &&
        request->stop_price <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
