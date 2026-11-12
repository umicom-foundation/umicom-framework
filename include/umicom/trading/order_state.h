/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_state.h
 *
 * PURPOSE:
 *   Validate the canonical order lifecycle state machine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates order state behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_ORDER_STATE_H
#define INCLUDE_UMICOM_TRADING_ORDER_STATE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the order transition allowed operation used by this module and its client
 * applications.
 */
/* Only declared states are valid, including for an unchanged state. */
int UmiOrderStatusValid(UmiOrderStatus state);
int umi_order_transition_allowed(UmiOrderStatus from, UmiOrderStatus to);
/* Complete a local cancellation from ACCEPTED or PARTIALLY_FILLED. A retained
 * CANCELLED state is an idempotent success, including at maximum revision.
 * Other states and exhausted new transitions preserve the complete order.
 * Accepted cancellation changes only status and version; partial fills remain.
 * Serialize access and admit the surrounding owner's revision/transaction first.
 * This value operation does not validate a request, send a broker command,
 * retain audit evidence, or establish an external cancellation acknowledgement. */
UmiStatus UmiOrderCancel(UmiOrder *order);
#ifdef __cplusplus
}
#endif
#endif
