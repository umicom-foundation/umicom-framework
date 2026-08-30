/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_market_data_event.c
 *
 * PURPOSE:
 *   Exercise normalise venue market-data sequence, instrument identity and event time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_data_event.h"
int main(void) {
    UmiFinancialId iid,vid;
    umi_trading_core_id_assign(&iid,"i");
    umi_trading_core_id_assign(&vid,"v");
     UmiTradingMarketDataEvent v;
     if(umi_trading_market_data_event_init(&v,&iid,&vid,1U,1000)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_market_data_event_valid(&v)) return 2;
     return 0;
}
