/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_state_machine.c
 *
 * PURPOSE:
 *   Exercise enforce deterministic order lifecycle state transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_state_machine.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingCoreOrderState s=UMI_TRADING_CORE_ORDER_PENDING_NEW;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_order_state_machine_apply(&s,UMI_TRADING_CORE_ORDER_OPEN)!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_order_state_machine_apply(&s,UMI_TRADING_CORE_ORDER_FILLED)!=UMI_STATUS_OK)return 2;
    return umi_trading_order_state_machine_apply(&s,UMI_TRADING_CORE_ORDER_OPEN)==UMI_STATUS_INVALID_STATE?0:3;
}
