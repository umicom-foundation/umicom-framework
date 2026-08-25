/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/double_entry_validation.c
 *
 * PURPOSE:
 *   Implement capture debit/credit control totals and balanced journal evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/double_entry_validation.h"
#include <string.h>
UmiStatus umi_accounting_double_entry_validation_init(UmiAccountingDoubleEntryValidation *value,
    const char *id,
    int64_t debit_minor,
    int64_t credit_minor,
    size_t line_count) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->debit_minor=debit_minor;
    value->credit_minor=credit_minor;
    value->line_count=line_count;
    return umi_accounting_double_entry_validation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_double_entry_validation_valid(const UmiAccountingDoubleEntryValidation *value) {
    return value!=NULL && (value->debit_minor>=0 && value->credit_minor>=0 && value->line_count>=2U);
}

bool umi_accounting_double_entry_validation_balanced(const UmiAccountingDoubleEntryValidation *value) {
    if(value==NULL) return (bool)0;
    return value->debit_minor==value->credit_minor;
}
