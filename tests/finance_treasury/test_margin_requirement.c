/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_margin_requirement.c
 *
 * PURPOSE:
 *   Exercise margin requirement validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_requirement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryMarginRequirement v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_margin_requirement_init(&v, "mr", 1000, 200, 50) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_margin_requirement_required_minor(&v)!=850)return 2;
    return 0;
}
