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
/**
 * Represent the accounting journal line data shared with callers of this public contract.
 */
typedef struct UmiAccountingJournalLine {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t debit_minor;
    int64_t credit_minor;
} UmiAccountingJournalLine;
/**
 * Initialise accounting journal line from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_journal_line_init(UmiAccountingJournalLine *value,
    const char *id,
    const char *account_id,
    int64_t debit_minor,
    int64_t credit_minor);
/**
 * Check that accounting journal line satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_journal_line_valid(const UmiAccountingJournalLine *value);
/**
 * Provide the accounting journal line signed minor operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_journal_line_signed_minor(const UmiAccountingJournalLine *value);
#ifdef __cplusplus
}
#endif
#endif
