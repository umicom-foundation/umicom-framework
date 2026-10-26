/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/ledger_balance.c
 *
 * PURPOSE:
 *   Implement calculate ledger-account net balance from debit/credit totals and normal side.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_balance.h"
#include <string.h>

/* Retain the original implementation for review. Complete validation now
 * precedes every arithmetic/control predicate, and initialization stages a
 * value before replacing the caller record. Valid results remain unchanged. */
#if 0
/*
 * Initialise accounting ledger balance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_ledger_balance_init(UmiAccountingLedgerBalance *value,
    const char *id,
    const char *account_id,
    UmiAccountingNormalSide normal_side,
    int64_t debit_minor,
    int64_t credit_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->account_id,account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->normal_side=normal_side;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    return umi_accounting_ledger_balance_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting ledger balance satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_ledger_balance_valid(const UmiAccountingLedgerBalance *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0 && (value->normal_side==UMI_ACCOUNTING_NORMAL_DEBIT||value->normal_side==UMI_ACCOUNTING_NORMAL_CREDIT));
}

/*
 * Provide the accounting ledger balance net minor operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_ledger_balance_net_minor(const UmiAccountingLedgerBalance *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return value->normal_side==UMI_ACCOUNTING_NORMAL_DEBIT ? value->debit_minor-value->credit_minor : value->credit_minor-value->debit_minor;
}

#endif

/* Own validation and arithmetic together so restored values cannot bypass the
 * invariants that make nonnegative debit/credit subtraction representable. */
UmiStatus umi_accounting_ledger_balance_init(UmiAccountingLedgerBalance *value,
    const char *id, const char *account_id, UmiAccountingNormalSide normal_side,
    int64_t debit_minor, int64_t credit_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAccountingLedgerBalance next = {0};
    UmiStatus status = umi_accounting_id_assign(&next.id, id);
    if (status == UMI_STATUS_OK) status = umi_accounting_id_assign(&next.account_id, account_id);
    if (status != UMI_STATUS_OK) return status;
    next.normal_side = normal_side; next.debit_minor = debit_minor; next.credit_minor = credit_minor;
    if (!umi_accounting_ledger_balance_valid(&next)) return UMI_STATUS_INVALID_ARGUMENT;
    *value = next; return UMI_STATUS_OK;
}
bool umi_accounting_ledger_balance_valid(const UmiAccountingLedgerBalance *value) {
    return value != NULL && umi_financial_id_is_valid(&value->id) &&
        umi_financial_id_is_valid(&value->account_id) && value->debit_minor >= 0 && value->credit_minor >= 0 &&
        (value->normal_side == UMI_ACCOUNTING_NORMAL_DEBIT || value->normal_side == UMI_ACCOUNTING_NORMAL_CREDIT);
}
UmiStatus umi_accounting_ledger_balance_net_minor_checked(
    const UmiAccountingLedgerBalance *value, int64_t *out_minor) {
    if (out_minor == NULL || !umi_accounting_ledger_balance_valid(value)) return UMI_STATUS_INVALID_ARGUMENT;
    *out_minor = value->normal_side == UMI_ACCOUNTING_NORMAL_DEBIT ?
        value->debit_minor - value->credit_minor : value->credit_minor - value->debit_minor;
    return UMI_STATUS_OK;
}
int64_t umi_accounting_ledger_balance_net_minor(const UmiAccountingLedgerBalance *value) {
    int64_t amount = 0;
    return umi_accounting_ledger_balance_net_minor_checked(value, &amount) == UMI_STATUS_OK ? amount : 0;
}
