/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_margin_profile.c
 *
 * PURPOSE:
 *   Exercise define conservative initial and maintenance margin ratios in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/margin_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingMarginProfile v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_margin_profile_init(&v,5000U,3000U,500U)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_margin_profile_valid(&v)) return 2;
     return 0;
}
