/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_fee_rule.c
 *
 * PURPOSE:
 *   Exercise payment fee rule validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_fee_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentFeeRule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_fee_rule_init(&v, "fee-rule", 10, 25U, 1000)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_payments_payment_fee_rule_fee_for_10000_minor(&v)!=35) return 2;
    return 0;
}
