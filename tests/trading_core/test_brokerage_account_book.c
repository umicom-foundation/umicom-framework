/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_brokerage_account_book.c
 *
 * PURPOSE:
 *   Exercise maintain bounded brokerage account state owned by the reusable trading core.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/brokerage_account_book.h"
int main(void) {

    UmiTradingBrokerageAccountBook b;
    UmiTradingBrokerageAccount a;
    UmiCurrency c={{'U','S','D','\0'}};
    umi_trading_brokerage_account_book_init(&b);
    umi_trading_brokerage_account_init(&a,"a",&c,false);
    if(umi_trading_brokerage_account_book_add(&b,&a)!=UMI_STATUS_OK)return 1;
    return umi_trading_brokerage_account_book_find(&b,&a.account_id)!=NULL?0:2;
}
