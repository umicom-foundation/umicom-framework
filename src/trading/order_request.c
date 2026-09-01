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
#include "umicom/finance/identifier.h"
#include "umicom/trading/instrument.h"

UmiStatus umi_order_request_validate(const UmiOrderRequest *request)
{
    if (request == NULL ||
        !umi_financial_id_valid(&request->client_order_id) ||
        !umi_financial_id_valid(&request->account_id) ||
        !umi_instrument_valid(&request->instrument) ||
        request->quantity <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if ((request->type == UMI_ORDER_LIMIT ||
         request->type == UMI_ORDER_STOP_LIMIT) &&
        request->limit_price <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if ((request->type == UMI_ORDER_STOP ||
         request->type == UMI_ORDER_STOP_LIMIT) &&
        request->stop_price <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
