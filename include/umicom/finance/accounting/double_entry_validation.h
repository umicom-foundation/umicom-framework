/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/double_entry_validation.h
 *
 * PURPOSE:
 *   Capture debit/credit control totals and balanced journal evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_DOUBLE_ENTRY_VALIDATION_H
#define UMICOM_FINANCE_ACCOUNTING_DOUBLE_ENTRY_VALIDATION_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting double entry validation data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingDoubleEntryValidation {
    UmiFinancialId id;
    int64_t debit_minor;
    int64_t credit_minor;
    size_t line_count;
} UmiAccountingDoubleEntryValidation;
/**
 * Initialise accounting double entry validation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_accounting_double_entry_validation_init(UmiAccountingDoubleEntryValidation *value,
    const char *id,
    int64_t debit_minor,
    int64_t credit_minor,
    size_t line_count);
/**
 * Check that accounting double entry validation satisfies its contract before another
 * service relies on it.
 */
bool umi_accounting_double_entry_validation_valid(const UmiAccountingDoubleEntryValidation *value);
/**
 * Provide the accounting double entry validation balanced operation used by this module
 * and its client applications.
 */
bool umi_accounting_double_entry_validation_balanced(const UmiAccountingDoubleEntryValidation *value);
#ifdef __cplusplus
}
#endif
#endif
