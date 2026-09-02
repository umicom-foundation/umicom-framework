/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_party.c
 *
 * PURPOSE:
 *   Exercise payment party validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_party.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentParty v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_party_init(&v, "party-1", "acct-1", "Debtor", "BANKGB2L")!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_party_valid(&v)) return 2;
    return 0;
}
