/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/double_entry_validation.h
 *
 * PURPOSE:
 *   Capture debit/credit control totals and balanced journal evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_DOUBLE_ENTRY_VALIDATION_H
#define UMICOM_FINANCE_ACCOUNTING_DOUBLE_ENTRY_VALIDATION_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingDoubleEntryValidation {
    UmiFinancialId id;
    int64_t debit_minor;
    int64_t credit_minor;
    size_t line_count;
} UmiAccountingDoubleEntryValidation;
UmiStatus umi_accounting_double_entry_validation_init(UmiAccountingDoubleEntryValidation *value,
    const char *id,
    int64_t debit_minor,
    int64_t credit_minor,
    size_t line_count);
bool umi_accounting_double_entry_validation_valid(const UmiAccountingDoubleEntryValidation *value);
bool umi_accounting_double_entry_validation_balanced(const UmiAccountingDoubleEntryValidation *value);
#ifdef __cplusplus
}
#endif
#endif
