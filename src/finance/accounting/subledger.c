/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/subledger.c
 *
 * PURPOSE:
 *   Implement represent sub-ledger control-account totals for reconciliation with the general ledger.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/subledger.h"
#include <string.h>

/* Preserve the former arithmetic for review. Complete validation and checked
 * exact arithmetic replace unchecked subtraction; valid values retain their
 * established result without saturation-based matching. */
#if 0
/*
 * Initialise accounting subledger from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_accounting_subledger_init(UmiAccountingSubledger *value,
    const char *id,
    const char *control_account_id,
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
    rc=umi_accounting_id_assign(&value->control_account_id,control_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    return umi_accounting_subledger_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting subledger satisfies its contract before another service relies on
 * it.
 */
bool umi_accounting_subledger_valid(const UmiAccountingSubledger *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0);
}

/*
 * Provide the accounting subledger net minor operation used by this module and its client
 * applications.
 */
int64_t umi_accounting_subledger_net_minor(const UmiAccountingSubledger *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return value->debit_minor-value->credit_minor;
}

#endif

/* Nonnegative ledger totals make subtraction representable only after their
 * invariants have been checked. Keep the validation at the arithmetic boundary. */
UmiStatus umi_accounting_subledger_init(UmiAccountingSubledger *value,
    const char *id, const char *control_account_id, int64_t debit_minor, int64_t credit_minor)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiAccountingSubledger next = {0};
    UmiStatus status = umi_accounting_id_assign(&next.id, id);
    if (status == UMI_STATUS_OK) {
        status = umi_accounting_id_assign(&next.control_account_id, control_account_id);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    next.debit_minor = debit_minor;
    next.credit_minor = credit_minor;
    if (!umi_accounting_subledger_valid(&next)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *value = next;
    return UMI_STATUS_OK;
}

bool umi_accounting_subledger_valid(const UmiAccountingSubledger *value)
{
    return value != NULL && umi_financial_id_is_valid(&value->id) &&
        umi_financial_id_is_valid(&value->control_account_id) &&
        value->debit_minor >= 0 && value->credit_minor >= 0;
}

UmiStatus umi_accounting_subledger_net_minor_checked(const UmiAccountingSubledger *value,
    int64_t *out_minor)
{
    if (out_minor == NULL || !umi_accounting_subledger_valid(value)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_minor = value->debit_minor - value->credit_minor;
    return UMI_STATUS_OK;
}

int64_t umi_accounting_subledger_net_minor(const UmiAccountingSubledger *value)
{
    int64_t amount = 0;
    return umi_accounting_subledger_net_minor_checked(value, &amount) == UMI_STATUS_OK ? amount : 0;
}
