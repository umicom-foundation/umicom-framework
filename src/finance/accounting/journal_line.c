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
