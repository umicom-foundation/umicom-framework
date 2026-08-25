/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_tick_size_rule.c
 *
 * PURPOSE:
 *   Exercise represent a price range and legal tick increment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/tick_size_rule.h"
int main(void) {
    UmiTradingTickSizeRule v;
     if(umi_trading_tick_size_rule_init(&v,0,100000,5)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_tick_size_rule_valid(&v)) return 2;
     if(umi_trading_tick_size_rule_aligned(&v)!=true) return 3;
     return 0;
}
