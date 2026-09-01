/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_state.c
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
 * This source implements the small deterministic core of order state. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_state.h"

int umi_order_transition_allowed(UmiOrderStatus from, UmiOrderStatus to)
{
    if (from == to) {
        return 1;
    }

    switch (from) {
    case UMI_ORDER_NEW:
        return to == UMI_ORDER_VALIDATED || to == UMI_ORDER_REJECTED;
    case UMI_ORDER_VALIDATED:
        return to == UMI_ORDER_ACCEPTED || to == UMI_ORDER_REJECTED;
    case UMI_ORDER_ACCEPTED:
        return to == UMI_ORDER_PARTIALLY_FILLED ||
               to == UMI_ORDER_FILLED ||
               to == UMI_ORDER_CANCELLED;
    case UMI_ORDER_PARTIALLY_FILLED:
        return to == UMI_ORDER_PARTIALLY_FILLED ||
               to == UMI_ORDER_FILLED ||
               to == UMI_ORDER_CANCELLED;
    default:
        return 0;
    }
}
