/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_cut_off_policy.c
 *
 * PURPOSE:
 *   Exercise cut off policy validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/cut_off_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsCutOffPolicy v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_cut_off_policy_init(&v, "cutoff-1", "rail-1", 1020U, true)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_payments_cut_off_policy_accepts_minute_600(&v)) return 2;
    return 0;
}
