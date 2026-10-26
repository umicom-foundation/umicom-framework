/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/journal_line.c
 *
 * PURPOSE:
 *   Implement represent one debit-or-credit journal line in minor units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_line.h"
#include <string.h>

/* Retain the earlier line implementation for review. The replacement checks identifier ownership and amount invariants before arithmetic, and stages initialization before publishing caller-owned state. */
#if 0
/*
 * Initialise accounting journal line from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_journal_line_init(UmiAccountingJournalLine *value,
    const char *id,
    const char *account_id,
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
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    return umi_accounting_journal_line_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting journal line satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_journal_line_valid(const UmiAccountingJournalLine *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0 && ((value->debit_minor>0)!=(value->credit_minor>0)));
}

/*
 * Provide the accounting journal line signed minor operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_journal_line_signed_minor(const UmiAccountingJournalLine *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return value->debit_minor-value->credit_minor;
}

#endif

/* Validate the complete fixed-size line before arithmetic. Constructors stage
 * a value first so failed input and aliased input strings cannot erase a record. */
UmiStatus umi_accounting_journal_line_init(UmiAccountingJournalLine *value,
    const char *id, const char *account_id, int64_t debit_minor, int64_t credit_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAccountingJournalLine next = {0};
    UmiStatus status = umi_accounting_id_assign(&next.id, id);
    if (status == UMI_STATUS_OK) status = umi_accounting_id_assign(&next.account_id, account_id);
    if (status != UMI_STATUS_OK) return status;
    next.debit_minor = debit_minor; next.credit_minor = credit_minor;
    if (!umi_accounting_journal_line_valid(&next)) return UMI_STATUS_INVALID_ARGUMENT;
    *value = next; return UMI_STATUS_OK;
}
bool umi_accounting_journal_line_valid(const UmiAccountingJournalLine *value) {
    return value != NULL && umi_financial_id_is_valid(&value->id) &&
        umi_financial_id_is_valid(&value->account_id) &&
        value->debit_minor >= 0 && value->credit_minor >= 0 &&
        ((value->debit_minor > 0) != (value->credit_minor > 0));
}
UmiStatus umi_accounting_journal_line_signed_minor_checked(
    const UmiAccountingJournalLine *value, int64_t *out_minor) {
    if (out_minor == NULL || !umi_accounting_journal_line_valid(value)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Exactly one nonnegative side is positive, so this difference is in
     * [-INT64_MAX, INT64_MAX] without an unrepresentable intermediate value. */
    *out_minor = value->debit_minor - value->credit_minor;
    return UMI_STATUS_OK;
}
int64_t umi_accounting_journal_line_signed_minor(const UmiAccountingJournalLine *value) {
    int64_t amount = 0;
    return umi_accounting_journal_line_signed_minor_checked(value, &amount) == UMI_STATUS_OK ? amount : 0;
}
