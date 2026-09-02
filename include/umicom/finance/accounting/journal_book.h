/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/journal_book.h
 *
 * PURPOSE:
 *   Store bounded validated journal entries and reject unbalanced postings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_JOURNAL_BOOK_H
#define UMICOM_FINANCE_ACCOUNTING_JOURNAL_BOOK_H
#include "umicom/finance/accounting/journal_entry.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting journal book data shared with callers of this public contract.
 */
typedef struct UmiAccountingJournalBook { UmiAccountingJournalEntry items[UMI_ACCOUNTING_MAX_ITEMS]; size_t count; } UmiAccountingJournalBook;
/**
 * Initialise accounting journal book from caller-provided values so later operations
 * receive a known state.
 */
void umi_accounting_journal_book_init(UmiAccountingJournalBook *value);
/**
 * Provide the accounting journal book post operation used by this module and its client
 * applications.
 */
UmiStatus umi_accounting_journal_book_post(UmiAccountingJournalBook *value,const UmiAccountingJournalEntry *entry);
/**
 * Find accounting journal book while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAccountingJournalEntry *umi_accounting_journal_book_find(const UmiAccountingJournalBook *value,const char *id);
#ifdef __cplusplus
}
#endif
#endif
