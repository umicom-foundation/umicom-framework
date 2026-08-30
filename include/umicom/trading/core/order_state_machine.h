/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_state_machine.h
 *
 * PURPOSE:
 *   Enforce deterministic order lifecycle state transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_STATE_MACHINE_H
#define UMICOM_TRADING_CORE_ORDER_STATE_MACHINE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Determine whether one normalised order state may transition to another. */
bool umi_trading_order_state_machine_allowed(UmiTradingCoreOrderState from,UmiTradingCoreOrderState to);
/* Apply a valid transition in place. */
UmiStatus umi_trading_order_state_machine_apply(UmiTradingCoreOrderState *state,UmiTradingCoreOrderState next);
#ifdef __cplusplus
}
#endif
#endif
