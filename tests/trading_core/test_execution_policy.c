/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_execution_policy.c
 *
 * PURPOSE:
 *   Exercise define venue-count, participation and urgency bounds for execution strategies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_policy.h"
int main(void) {
    UmiTradingExecutionPolicy v;
     if(umi_trading_execution_policy_init(&v,4U,1000U,50U)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_execution_policy_valid(&v)) return 2;
     return 0;
}
