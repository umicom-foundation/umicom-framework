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
typedef struct UmiAccountingJournalEntry { UmiFinancialId id; UmiFinancialDate accounting_date; UmiAccountingJournalStatus status; UmiAccountingJournalLine lines[UMI_ACCOUNTING_MAX_LINES]; size_t line_count; } UmiAccountingJournalEntry;
UmiStatus umi_accounting_journal_entry_init(UmiAccountingJournalEntry *value,const char *id,UmiFinancialDate accounting_date);
UmiStatus umi_accounting_journal_entry_add_line(UmiAccountingJournalEntry *value,const UmiAccountingJournalLine *line);
bool umi_accounting_journal_entry_valid(const UmiAccountingJournalEntry *value);
int64_t umi_accounting_journal_entry_total_debit(const UmiAccountingJournalEntry *value);
int64_t umi_accounting_journal_entry_total_credit(const UmiAccountingJournalEntry *value);
bool umi_accounting_journal_entry_balanced(const UmiAccountingJournalEntry *value);
#ifdef __cplusplus
}
#endif
#endif
