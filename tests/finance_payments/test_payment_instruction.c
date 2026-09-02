/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_instruction.c
 *
 * PURPOSE:
 *   Exercise payment instruction validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_instruction.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentInstruction v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_instruction_init(&v, "pay-1", "debtor", "creditor", "GBP", 1500, (UmiFinancialDate){2026,8U,25U}, UMI_PAYMENTS_APPROVED, "idem-1")!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_instruction_valid(&v)) return 2;
    return 0;
}
