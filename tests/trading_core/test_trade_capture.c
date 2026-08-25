/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_trade_capture.c
 *
 * PURPOSE:
 *   Exercise capture an exchange match as immutable trade evidence with unique identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/trade_capture.h"
int main(void) {

    UmiFinancialId b,s;
    UmiTradingTradeCapture t;
    umi_trading_core_id_assign(&b,"b");
    umi_trading_core_id_assign(&s,"s");
    if(umi_trading_trade_capture_init(&t,"t",&b,&s,100,5,1)!=UMI_STATUS_OK)return 1;
    return umi_trading_trade_capture_valid(&t)?0:2;
}
