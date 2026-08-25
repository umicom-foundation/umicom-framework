/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_exchange_trade_log.c
 *
 * PURPOSE:
 *   Exercise maintain sequence-stable bounded exchange trade capture evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/exchange_trade_log.h"
int main(void) {

    UmiTradingExchangeTradeLog l;
    UmiTradingTradeCapture t;
    UmiFinancialId b,s;
    umi_trading_exchange_trade_log_init(&l);
    umi_trading_core_id_assign(&b,"b");
    umi_trading_core_id_assign(&s,"s");
    umi_trading_trade_capture_init(&t,"t",&b,&s,100,1,1);
    if(umi_trading_exchange_trade_log_append(&l,&t)!=UMI_STATUS_OK)return 1;
    return umi_trading_exchange_trade_log_find(&l,&t.trade_id)!=NULL?0:2;
}
