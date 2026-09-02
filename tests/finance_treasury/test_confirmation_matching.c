/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_confirmation_matching.c
 *
 * PURPOSE:
 *   Exercise confirmation matching validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/confirmation_matching.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryConfirmationMatching v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_confirmation_matching_init(&v, "match", 8U, 8U) != UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!umi_treasury_confirmation_matching_exact(&v))return 2;
    return 0;
}
