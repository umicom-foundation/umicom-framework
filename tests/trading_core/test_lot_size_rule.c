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
int main(void) {
    UmiTradingLotSizeRule v;
     if(umi_trading_lot_size_rule_init(&v,1,1,1000)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_lot_size_rule_valid(&v)) return 2;
     return 0;
}
