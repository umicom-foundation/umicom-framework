/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_lot_size_rule.c
 *
 * PURPOSE:
 *   Exercise represent minimum and step quantities for a venue listing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/lot_size_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingLotSizeRule v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_lot_size_rule_init(&v,1,1,1000)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_lot_size_rule_valid(&v)) return 2;
     return 0;
}
