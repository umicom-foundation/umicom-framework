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
typedef struct UmiPaymentsPaymentReconciliation {
    UmiFinancialId id;
    int64_t statement_minor;
    int64_t ledger_minor;
    uint32_t unmatched_items;
} UmiPaymentsPaymentReconciliation;
UmiStatus umi_payments_payment_reconciliation_init(UmiPaymentsPaymentReconciliation *value,
    const char *id,
    int64_t statement_minor,
    int64_t ledger_minor,
    uint32_t unmatched_items);
bool umi_payments_payment_reconciliation_valid(const UmiPaymentsPaymentReconciliation *value);
int64_t umi_payments_payment_reconciliation_difference_minor(const UmiPaymentsPaymentReconciliation *value);
#ifdef __cplusplus
}
#endif
#endif
