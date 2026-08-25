/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_market_status.c
 *
 * PURPOSE:
 *   Exercise capture exchange phase, sequence and operational availability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_status.h"
int main(void) {
    UmiTradingMarketStatus v;
     if(umi_trading_market_status_init(&v,UMI_TRADING_CORE_PHASE_CONTINUOUS,1U,true)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_market_status_valid(&v)) return 2;
     return 0;
}
