/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_tick_size_rule.c
 *
 * PURPOSE:
 *   Exercise represent a price range and legal tick increment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/tick_size_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingTickSizeRule v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_tick_size_rule_init(&v,0,100000,5)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_tick_size_rule_valid(&v)) return 2;
     /* Apply this branch only when its contract condition is satisfied. */
     if(umi_trading_tick_size_rule_aligned(&v)!=true) return 3;
     return 0;
}
