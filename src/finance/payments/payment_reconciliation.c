/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_reconciliation.c
 *
 * PURPOSE:
 *   Implement compare external statement totals against internal payment-ledger totals.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_reconciliation.h"
#include <string.h>
UmiStatus umi_payments_payment_reconciliation_init(UmiPaymentsPaymentReconciliation *value,
    const char *id,
    int64_t statement_minor,
    int64_t ledger_minor,
    uint32_t unmatched_items) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->statement_minor=statement_minor;
    value->ledger_minor=ledger_minor;
    value->unmatched_items=unmatched_items;
    return umi_payments_payment_reconciliation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_reconciliation_valid(const UmiPaymentsPaymentReconciliation *value) {
    return value!=NULL && (true);
}

int64_t umi_payments_payment_reconciliation_difference_minor(const UmiPaymentsPaymentReconciliation *value) {
    if(value==NULL) return (int64_t)0;
    return value->statement_minor-value->ledger_minor;
}
