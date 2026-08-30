/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/journal_line.h
 *
 * PURPOSE:
 *   Represent one debit-or-credit journal line in minor units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_JOURNAL_LINE_H
#define UMICOM_FINANCE_ACCOUNTING_JOURNAL_LINE_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingJournalLine {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t debit_minor;
    int64_t credit_minor;
} UmiAccountingJournalLine;
UmiStatus umi_accounting_journal_line_init(UmiAccountingJournalLine *value,
    const char *id,
    const char *account_id,
    int64_t debit_minor,
    int64_t credit_minor);
bool umi_accounting_journal_line_valid(const UmiAccountingJournalLine *value);
int64_t umi_accounting_journal_line_signed_minor(const UmiAccountingJournalLine *value);
#ifdef __cplusplus
}
#endif
#endif
