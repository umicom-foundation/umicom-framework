/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_reconciliation.h
 *
 * PURPOSE:
 *   Compare external statement totals against internal payment-ledger totals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_RECONCILIATION_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_RECONCILIATION_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment reconciliation data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentReconciliation {
    UmiFinancialId id;
    int64_t statement_minor;
    int64_t ledger_minor;
    uint32_t unmatched_items;
} UmiPaymentsPaymentReconciliation;
/**
 * Initialise payments payment reconciliation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_payments_payment_reconciliation_init(UmiPaymentsPaymentReconciliation *value,
    const char *id,
    int64_t statement_minor,
    int64_t ledger_minor,
    uint32_t unmatched_items);
/**
 * Check that payments payment reconciliation satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_reconciliation_valid(const UmiPaymentsPaymentReconciliation *value);
/**
 * Provide the payments payment reconciliation difference minor operation used by this
 * module and its client applications.
 */
int64_t umi_payments_payment_reconciliation_difference_minor(const UmiPaymentsPaymentReconciliation *value);
#ifdef __cplusplus
}
#endif
#endif
