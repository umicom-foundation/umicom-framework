/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_exchange_service.c
 *
 * PURPOSE:
 *   Exercise aggregate venue session, order book, trade log and integrity state for thin exchange applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/exchange_service.h"
int main(void) {

    UmiTradingExchangeService s;
    UmiFinancialId v;
    umi_trading_core_id_assign(&v,"v");
    if(umi_trading_exchange_service_init(&s,&v)!=UMI_STATUS_OK)return 1;
    return umi_trading_exchange_service_refresh(&s,0)?0:2;
}
