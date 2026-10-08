/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_state_machine.c
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
#include "umicom/trading/core/order_state_machine.h"

/* Validate both enum operands before considering a self-transition. This keeps
 * invalid restored values from being accepted simply because they are equal. */
static bool CoreOrderStateKnown(UmiTradingCoreOrderState state)
{
    switch(state) {
    case UMI_TRADING_CORE_ORDER_PENDING_NEW: case UMI_TRADING_CORE_ORDER_OPEN:
    case UMI_TRADING_CORE_ORDER_PARTIAL: case UMI_TRADING_CORE_ORDER_FILLED:
    case UMI_TRADING_CORE_ORDER_PENDING_CANCEL: case UMI_TRADING_CORE_ORDER_CANCELLED:
    case UMI_TRADING_CORE_ORDER_REJECTED: case UMI_TRADING_CORE_ORDER_EXPIRED:return true;
    default:return false;
    }
}
/* Determine whether one normalised order state may transition to another. */
/* The earlier equality-first predicate is retained for review. The guarded
 * replacement preserves all declared transitions and rejects unknown states. */
#if 0
bool umi_trading_order_state_machine_allowed(UmiTradingCoreOrderState from,UmiTradingCoreOrderState to){/* Apply this operation only while the related capability or state is available. */ if(from==to)return true;/* Select the behaviour associated with the requested command or state value. */ switch(from){case UMI_TRADING_CORE_ORDER_PENDING_NEW:return to==UMI_TRADING_CORE_ORDER_OPEN||to==UMI_TRADING_CORE_ORDER_REJECTED;case UMI_TRADING_CORE_ORDER_OPEN:return to==UMI_TRADING_CORE_ORDER_PARTIAL||to==UMI_TRADING_CORE_ORDER_FILLED||to==UMI_TRADING_CORE_ORDER_PENDING_CANCEL||to==UMI_TRADING_CORE_ORDER_EXPIRED||to==UMI_TRADING_CORE_ORDER_REJECTED;case UMI_TRADING_CORE_ORDER_PARTIAL:return to==UMI_TRADING_CORE_ORDER_PARTIAL||to==UMI_TRADING_CORE_ORDER_FILLED||to==UMI_TRADING_CORE_ORDER_PENDING_CANCEL||to==UMI_TRADING_CORE_ORDER_EXPIRED;case UMI_TRADING_CORE_ORDER_PENDING_CANCEL:return to==UMI_TRADING_CORE_ORDER_CANCELLED||to==UMI_TRADING_CORE_ORDER_FILLED||to==UMI_TRADING_CORE_ORDER_PARTIAL;default:return false;}}
#endif
bool umi_trading_order_state_machine_allowed(UmiTradingCoreOrderState from,UmiTradingCoreOrderState to){if(!CoreOrderStateKnown(from)||!CoreOrderStateKnown(to))return false;/* Apply this operation only while the related capability or state is available. */ if(from==to)return true;/* Select the behaviour associated with the requested command or state value. */ switch(from){case UMI_TRADING_CORE_ORDER_PENDING_NEW:return to==UMI_TRADING_CORE_ORDER_OPEN||to==UMI_TRADING_CORE_ORDER_REJECTED;case UMI_TRADING_CORE_ORDER_OPEN:return to==UMI_TRADING_CORE_ORDER_PARTIAL||to==UMI_TRADING_CORE_ORDER_FILLED||to==UMI_TRADING_CORE_ORDER_PENDING_CANCEL||to==UMI_TRADING_CORE_ORDER_EXPIRED||to==UMI_TRADING_CORE_ORDER_REJECTED;case UMI_TRADING_CORE_ORDER_PARTIAL:return to==UMI_TRADING_CORE_ORDER_PARTIAL||to==UMI_TRADING_CORE_ORDER_FILLED||to==UMI_TRADING_CORE_ORDER_PENDING_CANCEL||to==UMI_TRADING_CORE_ORDER_EXPIRED;case UMI_TRADING_CORE_ORDER_PENDING_CANCEL:return to==UMI_TRADING_CORE_ORDER_CANCELLED||to==UMI_TRADING_CORE_ORDER_FILLED||to==UMI_TRADING_CORE_ORDER_PARTIAL;default:return false;}}
/* Apply a valid transition in place. */
UmiStatus umi_trading_order_state_machine_apply(UmiTradingCoreOrderState *state,UmiTradingCoreOrderState next){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(state==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_trading_order_state_machine_allowed(*state,next))return UMI_STATUS_INVALID_STATE;*state=next;return UMI_STATUS_OK;}
