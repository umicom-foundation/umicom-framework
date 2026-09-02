/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_exchange_service.c
 *
 * PURPOSE:
 *   Exercise aggregate venue session, order book, trade log and integrity state for thin exchange applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/exchange_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingExchangeService s;
    UmiFinancialId v;
    umi_trading_core_id_assign(&v,"v");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_exchange_service_init(&s,&v)!=UMI_STATUS_OK)return 1;
    return umi_trading_exchange_service_refresh(&s,0)?0:2;
}
