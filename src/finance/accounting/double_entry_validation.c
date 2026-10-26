/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/double_entry_validation.c
 *
 * PURPOSE:
 *   Implement capture debit/credit control totals and balanced journal evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/double_entry_validation.h"
#include <string.h>

/* Retain the original implementation for review. Complete validation now
 * precedes every arithmetic/control predicate, and initialization stages a
 * value before replacing the caller record. Valid results remain unchanged. */
#if 0
/*
 * Initialise accounting double entry validation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_accounting_double_entry_validation_init(UmiAccountingDoubleEntryValidation *value,
    const char *id,
    int64_t debit_minor,
    int64_t credit_minor,
    size_t line_count) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    value->line_count=line_count;
    return umi_accounting_double_entry_validation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting double entry validation satisfies its contract before another
 * service relies on it.
 */
bool umi_accounting_double_entry_validation_valid(const UmiAccountingDoubleEntryValidation *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0 && value->line_count>=2U);
}

/*
 * Provide the accounting double entry validation balanced operation used by this module
 * and its client applications.
 */
bool umi_accounting_double_entry_validation_balanced(const UmiAccountingDoubleEntryValidation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->debit_minor==value->credit_minor;
}

#endif

/* Validate restored control totals before comparing them. Equality alone must
 * not turn malformed negative values or invalid identifiers into valid evidence. */
UmiStatus umi_accounting_double_entry_validation_init(UmiAccountingDoubleEntryValidation *value, const char *id,
    int64_t debit_minor, int64_t credit_minor, size_t line_count) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAccountingDoubleEntryValidation next = {0};
    UmiStatus status = umi_accounting_id_assign(&next.id, id);
    if (status != UMI_STATUS_OK) return status;
    next.debit_minor = debit_minor; next.credit_minor = credit_minor; next.line_count = line_count;
    if (!umi_accounting_double_entry_validation_valid(&next)) return UMI_STATUS_INVALID_ARGUMENT;
    *value = next; return UMI_STATUS_OK;
}
bool umi_accounting_double_entry_validation_valid(const UmiAccountingDoubleEntryValidation *value) {
    return value != NULL && umi_financial_id_is_valid(&value->id) &&
        value->debit_minor >= 0 && value->credit_minor >= 0 && value->line_count >= 2U;
}
bool umi_accounting_double_entry_validation_balanced(const UmiAccountingDoubleEntryValidation *value) {
    return umi_accounting_double_entry_validation_valid(value) && value->debit_minor == value->credit_minor;
}
