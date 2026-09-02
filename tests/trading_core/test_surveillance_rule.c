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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingSurveillanceRule v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_surveillance_rule_init(&v,3U,60U,UMI_TRADING_CORE_WARNING)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_surveillance_rule_valid(&v)) return 2;
     return 0;
}
