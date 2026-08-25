/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_consolidated_trade.c
 *
 * PURPOSE:
 *   Exercise represent a venue trade print with integer-normalised price and quantity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/consolidated_trade.h"
int main(void) {
    UmiFinancialId iid,vid;
    umi_trading_core_id_assign(&iid,"i");
    umi_trading_core_id_assign(&vid,"v");
     UmiTradingConsolidatedTrade v;
     if(umi_trading_consolidated_trade_init(&v,&iid,&vid,100,5,1000)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_consolidated_trade_valid(&v)) return 2;
     return 0;
}
