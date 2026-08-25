/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_brokerage_account.c
 *
 * PURPOSE:
 *   Exercise define brokerage account identity, base currency and account trading state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/brokerage_account.h"
int main(void) {

    UmiTradingBrokerageAccount a;
    UmiCurrency c={{'U','S','D','\0'}};
    if(umi_trading_brokerage_account_init(&a,"a",&c,true)!=UMI_STATUS_OK)return 1;
    return a.margin_enabled?0:2;
}
