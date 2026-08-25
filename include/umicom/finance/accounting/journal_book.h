/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/journal_book.h
 *
 * PURPOSE:
 *   Store bounded validated journal entries and reject unbalanced postings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_JOURNAL_BOOK_H
#define UMICOM_FINANCE_ACCOUNTING_JOURNAL_BOOK_H
#include "umicom/finance/accounting/journal_entry.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingJournalBook { UmiAccountingJournalEntry items[UMI_ACCOUNTING_MAX_ITEMS]; size_t count; } UmiAccountingJournalBook;
void umi_accounting_journal_book_init(UmiAccountingJournalBook *value);
UmiStatus umi_accounting_journal_book_post(UmiAccountingJournalBook *value,const UmiAccountingJournalEntry *entry);
const UmiAccountingJournalEntry *umi_accounting_journal_book_find(const UmiAccountingJournalBook *value,const char *id);
#ifdef __cplusplus
}
#endif
#endif
