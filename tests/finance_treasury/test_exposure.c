/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_exposure.c
 *
 * PURPOSE:
 *   Exercise exposure validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/exposure.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryExposure v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_exposure_init(&v, "exp", 1000, -600) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_exposure_net_absolute_minor(&v)!=600)return 2;
    return 0;
}
