/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_payment_control_book.c
 *
 * PURPOSE:
 *   Verify approval gating and terminal payment lifecycle states.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/financial_platform/payment_control_book.h"

int main(void)
{
    UmiFinancialPaymentControlBook book;
    UmiFinancialPaymentControl payment = {0};
    umi_financial_payment_control_book_init(&book);
    (void)strcpy(payment.paymentId, "payment-1");
    (void)strcpy(payment.debtorAccountId, "acct-1");
    (void)strcpy(payment.creditorPartyId, "party-2");
    (void)strcpy(payment.currency, "GBP");
    payment.amount = 100.0;
    payment.version = 1U;
    assert(umi_financial_payment_control_book_add(&book, &payment) == UMI_STATUS_OK);
    assert(umi_financial_payment_control_book_transition(
               &book, "payment-1", UMI_FINANCIAL_PAYMENT_SENT, 2U) ==
           UMI_STATUS_PERMISSION_DENIED);
    book.payments[0].approved = 1;
    assert(umi_financial_payment_control_book_transition(
               &book, "payment-1", UMI_FINANCIAL_PAYMENT_SENT, 2U) ==
           UMI_STATUS_OK);
    assert(umi_financial_payment_control_book_transition(
               &book, "payment-1", UMI_FINANCIAL_PAYMENT_SETTLED, 3U) ==
           UMI_STATUS_OK);
    return 0;
}
