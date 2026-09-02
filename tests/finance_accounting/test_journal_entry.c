/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_journal_entry.c
 *
 * PURPOSE:
 *   Verify balanced debit/credit journal lines and immutable-ready journal structure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_entry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiAccountingJournalEntry e; UmiAccountingJournalLine d,c; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_accounting_journal_entry_init(&e,"je-1",(UmiFinancialDate){2026,8U,25U})!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_accounting_journal_line_init(&d,"l1","1000",1000,0)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_accounting_journal_line_init(&c,"l2","4000",0,1000)!=UMI_STATUS_OK)return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_accounting_journal_entry_add_line(&e,&d)!=UMI_STATUS_OK)return 4; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_accounting_journal_entry_add_line(&e,&c)!=UMI_STATUS_OK)return 5; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_accounting_journal_entry_balanced(&e))return 6; return 0; }
