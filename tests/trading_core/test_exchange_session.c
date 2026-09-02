/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_exchange_session.c
 *
 * PURPOSE:
 *   Exercise coordinate market phase, sequence and circuit-breaker state for a reusable exchange session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/exchange_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiFinancialId v;
    UmiTradingExchangeSession s;
    umi_trading_core_id_assign(&v,"v");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_exchange_session_init(&s,&v,500U,60U)!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_exchange_session_transition(&s,UMI_TRADING_CORE_PHASE_PREOPEN)!=UMI_STATUS_OK)return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_exchange_session_transition(&s,UMI_TRADING_CORE_PHASE_CONTINUOUS)!=UMI_STATUS_OK)return 3;
    return umi_trading_exchange_session_matching_enabled(&s,0)?0:4;
}
