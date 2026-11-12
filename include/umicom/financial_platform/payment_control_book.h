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

/* Validate complete caller-owned storage before traversal: bounded terminated
 * text, required identities, finite numeric observations, known states/boolean
 * flags and unique keys. Count must fit the array and revision must be nonzero.
 * These references do not replace canonical domain engines or authenticate
 * permissions. Keep inputs stable on one owner thread and outputs disjoint.
 * Borrowed find results last until the next mutation; queries inspect at most
 * 64 readable bytes and must terminate. Unused rows/text tails are ignored. */
UmiStatus umi_financial_payment_control_validate(const UmiFinancialPaymentControl *row);
UmiStatus umi_financial_payment_control_book_validate(const UmiFinancialPaymentControlBook *owner);
/* Imported observations may use any known state. Transition retains the
 * existing known-state edges, terminal rules and strictly increasing versions.
 * Updating these local values does not perform a financial transaction. */
/* SENT/SETTLED transitions still require the local approved flag. That flag
 * is caller-supplied evidence, not an authenticated payment authorization. */
void umi_financial_payment_control_book_init(UmiFinancialPaymentControlBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_financial_payment_control_book_add(
    UmiFinancialPaymentControlBook *book,
    const UmiFinancialPaymentControl *payment);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
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
