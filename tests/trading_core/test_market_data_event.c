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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFinancialId iid,vid;
    umi_trading_core_id_assign(&iid,"i");
    umi_trading_core_id_assign(&vid,"v");
     UmiTradingMarketDataEvent v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_market_data_event_init(&v,&iid,&vid,1U,1000)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_market_data_event_valid(&v)) return 2;
     return 0;
}
