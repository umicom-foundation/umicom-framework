/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_exchange_session.c
 *
 * PURPOSE:
 *   Exercise coordinate market phase, sequence and circuit-breaker state for a reusable exchange session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/exchange_session.h"
int main(void) {

    UmiFinancialId v;
    UmiTradingExchangeSession s;
    umi_trading_core_id_assign(&v,"v");
    if(umi_trading_exchange_session_init(&s,&v,500U,60U)!=UMI_STATUS_OK)return 1;
    if(umi_trading_exchange_session_transition(&s,UMI_TRADING_CORE_PHASE_PREOPEN)!=UMI_STATUS_OK)return 2;
    if(umi_trading_exchange_session_transition(&s,UMI_TRADING_CORE_PHASE_CONTINUOUS)!=UMI_STATUS_OK)return 3;
    return umi_trading_exchange_session_matching_enabled(&s,0)?0:4;
}
