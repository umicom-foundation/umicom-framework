/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_cash_ledger.c
 *
 * PURPOSE:
 *   Exercise maintain bounded integer cash balances by currency for brokerage settlement state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/cash_ledger.h"
int main(void) {

    UmiTradingCashLedger l;
    UmiCurrency c={{'U','S','D','\0'}};
    umi_trading_cash_ledger_init(&l);
    if(umi_trading_cash_ledger_apply(&l,&c,100)!=UMI_STATUS_OK)return 1;
    if(umi_trading_cash_ledger_apply(&l,&c,-30)!=UMI_STATUS_OK)return 2;
    return umi_trading_cash_ledger_get(&l,&c)==70?0:3;
}
