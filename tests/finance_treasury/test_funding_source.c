/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_funding_source.c
 *
 * PURPOSE:
 *   Exercise funding source validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_source.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryFundingSource v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_funding_source_init(&v, "facility", 1000, 250, 125) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_funding_source_available_minor(&v)!=750)return 2;
    return 0;
}
