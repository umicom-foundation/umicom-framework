/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_surveillance_rule.c
 *
 * PURPOSE:
 *   Exercise define reusable market-surveillance thresholds and alert severity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/surveillance_rule.h"
int main(void) {
    UmiTradingSurveillanceRule v;
     if(umi_trading_surveillance_rule_init(&v,3U,60U,UMI_TRADING_CORE_WARNING)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_surveillance_rule_valid(&v)) return 2;
     return 0;
}
