/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/payment_control_book.h
 *
 * PURPOSE:
 *   Track cross-domain payment-control lifecycle, approval and settlement
 *   references without replacing the canonical payments engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_PAYMENT_CONTROL_BOOK_H
#define UMICOM_FINANCIAL_PLATFORM_PAYMENT_CONTROL_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY 512U

typedef enum UmiFinancialPaymentControlState {
    UMI_FINANCIAL_PAYMENT_DRAFT = 0,
    UMI_FINANCIAL_PAYMENT_APPROVAL_PENDING = 1,
    UMI_FINANCIAL_PAYMENT_APPROVED = 2,
    UMI_FINANCIAL_PAYMENT_SENT = 3,
    UMI_FINANCIAL_PAYMENT_SETTLED = 4,
    UMI_FINANCIAL_PAYMENT_REJECTED = 5
} UmiFinancialPaymentControlState;

typedef struct UmiFinancialPaymentControl {
    char paymentId[64];
    char debtorAccountId[64];
    char creditorPartyId[64];
    char currency[8];
    double amount;
    UmiFinancialPaymentControlState state;
    uint64_t version;
    int approved;
} UmiFinancialPaymentControl;

typedef struct UmiFinancialPaymentControlBook {
    UmiFinancialPaymentControl payments[UMI_FINANCIAL_PAYMENT_CONTROL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiFinancialPaymentControlBook;

void umi_financial_payment_control_book_init(UmiFinancialPaymentControlBook *book);
UmiStatus umi_financial_payment_control_book_add(
    UmiFinancialPaymentControlBook *book,
    const UmiFinancialPaymentControl *payment);
UmiStatus umi_financial_payment_control_book_transition(
    UmiFinancialPaymentControlBook *book,
    const char *paymentId,
    UmiFinancialPaymentControlState state,
    uint64_t version);
const UmiFinancialPaymentControl *umi_financial_payment_control_book_find(
    const UmiFinancialPaymentControlBook *book,
    const char *paymentId);

#ifdef __cplusplus
}
#endif
#endif
