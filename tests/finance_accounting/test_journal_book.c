/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_journal_book.c
 *
 * PURPOSE:
 *   Verify journal-book posting only accepts balanced entries and prevents duplicates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_book.h"
int main(void){ UmiAccountingJournalBook b; UmiAccountingJournalEntry e; UmiAccountingJournalLine d,c; umi_accounting_journal_book_init(&b); if(umi_accounting_journal_entry_init(&e,"je-1",(UmiFinancialDate){2026,8U,25U})!=UMI_STATUS_OK)return 1; if(umi_accounting_journal_line_init(&d,"d","1000",100,0)!=UMI_STATUS_OK)return 2; if(umi_accounting_journal_line_init(&c,"c","4000",0,100)!=UMI_STATUS_OK)return 3; if(umi_accounting_journal_entry_add_line(&e,&d)!=UMI_STATUS_OK||umi_accounting_journal_entry_add_line(&e,&c)!=UMI_STATUS_OK)return 4; if(umi_accounting_journal_book_post(&b,&e)!=UMI_STATUS_OK)return 5; if(umi_accounting_journal_book_post(&b,&e)!=UMI_STATUS_ALREADY_EXISTS)return 6; return 0; }
