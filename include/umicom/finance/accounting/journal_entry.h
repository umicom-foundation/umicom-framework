/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/journal_entry.h
 *
 * PURPOSE:
 *   Maintain immutable-ready balanced journal entries with bounded debit and credit lines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_JOURNAL_ENTRY_H
#define UMICOM_FINANCE_ACCOUNTING_JOURNAL_ENTRY_H
#include "umicom/finance/accounting/journal_line.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting journal entry data shared with callers of this public contract.
 */
typedef struct UmiAccountingJournalEntry { UmiFinancialId id; UmiFinancialDate accounting_date; UmiAccountingJournalStatus status; UmiAccountingJournalLine lines[UMI_ACCOUNTING_MAX_LINES]; size_t line_count; } UmiAccountingJournalEntry;
/**
 * Initialise accounting journal entry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_journal_entry_init(UmiAccountingJournalEntry *value,const char *id,UmiFinancialDate accounting_date);
/**
 * Provide the accounting journal entry add line operation used by this module and its
 * client applications.
 */
UmiStatus umi_accounting_journal_entry_add_line(UmiAccountingJournalEntry *value,const UmiAccountingJournalLine *line);
/**
 * Check that accounting journal entry satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_journal_entry_valid(const UmiAccountingJournalEntry *value);
/**
 * Provide the accounting journal entry total debit operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_journal_entry_total_debit(const UmiAccountingJournalEntry *value);
/**
 * Provide the accounting journal entry total credit operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_journal_entry_total_credit(const UmiAccountingJournalEntry *value);
/**
 * Provide the accounting journal entry balanced operation used by this module and its
 * client applications.
 */
bool umi_accounting_journal_entry_balanced(const UmiAccountingJournalEntry *value);
#ifdef __cplusplus
}
#endif
#endif
