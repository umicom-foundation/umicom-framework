/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_approval_policy.c
 *
 * PURPOSE:
 *   Exercise approval policy validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/approval_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsApprovalPolicy v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_approval_policy_init(&v, "approval-1", 1U, 1U, 1U, 1U, true)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_payments_approval_policy_satisfied(&v)) return 2;
    return 0;
}
