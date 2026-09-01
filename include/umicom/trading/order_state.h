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
int umi_order_transition_allowed(UmiOrderStatus from, UmiOrderStatus to);
#ifdef __cplusplus
}
#endif
#endif
