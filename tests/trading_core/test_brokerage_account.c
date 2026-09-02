/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_brokerage_account.c
 *
 * PURPOSE:
 *   Exercise define brokerage account identity, base currency and account trading state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/brokerage_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingBrokerageAccount a;
    UmiCurrency c={{'U','S','D','\0'}};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_brokerage_account_init(&a,"a",&c,true)!=UMI_STATUS_OK)return 1;
    return a.margin_enabled?0:2;
}
