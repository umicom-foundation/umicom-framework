/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/payment_control_book.c
 *
 * PURPOSE:
 *   Implement controlled payment lifecycle transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/payment_control_book.h"
#include <string.h>

void umi_financial_payment_control_book_init(UmiFinancialPaymentControlBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiFinancialPaymentControl *umi_financial_payment_control_book_find(
    const UmiFinancialPaymentControlBook *book,
    const char *paymentId)
{
    size_t i;
    if (book == NULL || paymentId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->payments[i].paymentId, paymentId) == 0)
            return &book->payments[i];
    return NULL;
}

UmiStatus umi_financial_payment_control_book_add(
    UmiFinancialPaymentControlBook *book,
    const UmiFinancialPaymentControl *payment)
{
    if (book == NULL || payment == NULL || payment->paymentId[0] == '\0' ||
        payment->debtorAccountId[0] == '\0' || payment->creditorPartyId[0] == '\0' ||
        payment->currency[0] == '\0' || payment->amount <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_payment_control_book_find(book, payment->paymentId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->payments[book->count++] = *payment;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_payment_control_book_transition(
    UmiFinancialPaymentControlBook *book,
    const char *paymentId,
    UmiFinancialPaymentControlState state,
    uint64_t version)
{
    size_t i;
    if (book == NULL || paymentId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialPaymentControl *payment = &book->payments[i];
        if (strcmp(payment->paymentId, paymentId) == 0) {
            if (version <= payment->version) return UMI_STATUS_INVALID_STATE;
            if (payment->state == UMI_FINANCIAL_PAYMENT_SETTLED ||
                payment->state == UMI_FINANCIAL_PAYMENT_REJECTED)
                return UMI_STATUS_INVALID_STATE;
            if ((state == UMI_FINANCIAL_PAYMENT_SENT ||
                 state == UMI_FINANCIAL_PAYMENT_SETTLED) &&
                !payment->approved)
                return UMI_STATUS_PERMISSION_DENIED;
            payment->state = state;
            payment->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
