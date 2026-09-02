/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/journal_entry.c
 *
 * PURPOSE:
 *   Implement bounded journal line insertion, totals and double-entry balance checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_entry.h"
#include <string.h>
/*
 * Initialise accounting journal entry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_journal_entry_init(UmiAccountingJournalEntry *value,const char *id,UmiFinancialDate accounting_date){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(value,0,sizeof *value); UmiStatus s=umi_accounting_id_assign(&value->id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; value->accounting_date=accounting_date; value->status=UMI_ACCOUNTING_JOURNAL_DRAFT; return umi_accounting_journal_entry_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT; }
/*
 * Check that accounting journal entry satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_journal_entry_valid(const UmiAccountingJournalEntry *value){ return value!=NULL&&umi_financial_id_is_valid(&value->id)&&umi_financial_date_is_valid(value->accounting_date)&&value->line_count<=UMI_ACCOUNTING_MAX_LINES; }
/*
 * Provide the accounting journal entry add line operation used by this module and its
 * client applications.
 */
UmiStatus umi_accounting_journal_entry_add_line(UmiAccountingJournalEntry *value,const UmiAccountingJournalLine *line){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_accounting_journal_entry_valid(value)||line==NULL||!umi_accounting_journal_line_valid(line))return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value->status!=UMI_ACCOUNTING_JOURNAL_DRAFT)return UMI_STATUS_INVALID_STATE; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value->line_count>=UMI_ACCOUNTING_MAX_LINES)return UMI_STATUS_CAPACITY_EXCEEDED; value->lines[value->line_count++]=*line; return UMI_STATUS_OK; }
/*
 * Provide the accounting journal entry total debit operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_journal_entry_total_debit(const UmiAccountingJournalEntry *value){ int64_t t=0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL)return 0; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->line_count;++i)t+=value->lines[i].debit_minor; return t; }
/*
 * Provide the accounting journal entry total credit operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_journal_entry_total_credit(const UmiAccountingJournalEntry *value){ int64_t t=0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL)return 0; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->line_count;++i)t+=value->lines[i].credit_minor; return t; }
/*
 * Provide the accounting journal entry balanced operation used by this module and its
 * client applications.
 */
bool umi_accounting_journal_entry_balanced(const UmiAccountingJournalEntry *value){ return value!=NULL&&value->line_count>=2U&&umi_accounting_journal_entry_total_debit(value)==umi_accounting_journal_entry_total_credit(value); }
