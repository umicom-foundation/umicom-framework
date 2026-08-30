/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_matching_policy.c
 *
 * PURPOSE:
 *   Exercise define common exchange matching priorities and self-trade prevention behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/matching_policy.h"
int main(void) {
    UmiTradingMatchingPolicy v;
     if(umi_trading_matching_policy_init(&v,true,true,32U)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_matching_policy_valid(&v)) return 2;
     return 0;
}
