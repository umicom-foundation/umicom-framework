/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_matching_engine.c
 *
 * PURPOSE:
 *   Exercise match crossing buy and sell interest under a reusable price-time policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/matching_engine.h"
int main(void) {

    UmiTradingMatchingPolicy p;
    UmiTradingMatchRequest r={101,10,100,7};
    UmiTradingMatchResult m;
    umi_trading_matching_policy_init(&p,true,true,10U);
    if(umi_trading_matching_engine_match(&p,&r,&m)!=UMI_STATUS_OK)return 1;
    return m.matched&&m.execution_lots==7&&m.execution_ticks==100?0:2;
}
