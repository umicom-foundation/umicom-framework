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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingMatchingPolicy v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_matching_policy_init(&v,true,true,32U)!=UMI_STATUS_OK) return 1;
     /* Use the stable identifier comparison to choose the matching record or policy. */
     if(!umi_trading_matching_policy_valid(&v)) return 2;
     return 0;
}
