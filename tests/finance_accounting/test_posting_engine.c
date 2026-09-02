/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_posting_engine.c
 *
 * PURPOSE:
 *   Verify canonical accounting events generate deterministic balanced two-sided journal entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_engine.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiAccountingPostingEngine e; UmiAccountingAccountingEvent ev; UmiAccountingPostingRule r; UmiAccountingJournalEntry j; umi_accounting_posting_engine_init(&e); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_accounting_accounting_event_init(&ev,"evt-1","CASH_RECEIPT",(UmiFinancialDate){2026,8U,25U},1000,"GBP")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_accounting_posting_rule_init(&r,"rule-1","CASH_RECEIPT","1000","4000",true)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_accounting_posting_engine_generate(&e,&ev,&r,"je-1",&j)!=UMI_STATUS_OK)return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_accounting_journal_entry_balanced(&j))return 4; return 0; }
