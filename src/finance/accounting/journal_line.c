/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/journal_line.c
 *
 * PURPOSE:
 *   Implement represent one debit-or-credit journal line in minor units.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_line.h"
#include <string.h>
UmiStatus umi_accounting_journal_line_init(UmiAccountingJournalLine *value,
    const char *id,
    const char *account_id,
    int64_t debit_minor,
    int64_t credit_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->account_id,account_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    return umi_accounting_journal_line_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_journal_line_valid(const UmiAccountingJournalLine *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0 && ((value->debit_minor>0)!=(value->credit_minor>0)));
}

int64_t umi_accounting_journal_line_signed_minor(const UmiAccountingJournalLine *value) {
    if(value==NULL) return (int64_t)0;
    return value->debit_minor-value->credit_minor;
}
