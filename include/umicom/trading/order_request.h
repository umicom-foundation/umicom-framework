/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_request.h
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
 * This public contract isolates order request behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ORDER_REQUEST_H
#define INCLUDE_UMICOM_TRADING_ORDER_REQUEST_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Check that order request satisfies its contract before another service relies on it.
 */
UmiStatus umi_order_request_validate(const UmiOrderRequest *request);
#ifdef __cplusplus
}
#endif
#endif
