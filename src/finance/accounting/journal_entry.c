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
UmiStatus umi_accounting_journal_entry_init(UmiAccountingJournalEntry *value,const char *id,UmiFinancialDate accounting_date){ if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(value,0,sizeof *value); UmiStatus s=umi_accounting_id_assign(&value->id,id); if(s!=UMI_STATUS_OK)return s; value->accounting_date=accounting_date; value->status=UMI_ACCOUNTING_JOURNAL_DRAFT; return umi_accounting_journal_entry_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT; }
bool umi_accounting_journal_entry_valid(const UmiAccountingJournalEntry *value){ return value!=NULL&&umi_financial_id_is_valid(&value->id)&&umi_financial_date_is_valid(value->accounting_date)&&value->line_count<=UMI_ACCOUNTING_MAX_LINES; }
UmiStatus umi_accounting_journal_entry_add_line(UmiAccountingJournalEntry *value,const UmiAccountingJournalLine *line){ if(!umi_accounting_journal_entry_valid(value)||line==NULL||!umi_accounting_journal_line_valid(line))return UMI_STATUS_INVALID_ARGUMENT; if(value->status!=UMI_ACCOUNTING_JOURNAL_DRAFT)return UMI_STATUS_INVALID_STATE; if(value->line_count>=UMI_ACCOUNTING_MAX_LINES)return UMI_STATUS_CAPACITY_EXCEEDED; value->lines[value->line_count++]=*line; return UMI_STATUS_OK; }
int64_t umi_accounting_journal_entry_total_debit(const UmiAccountingJournalEntry *value){ int64_t t=0; if(value==NULL)return 0; for(size_t i=0U;i<value->line_count;++i)t+=value->lines[i].debit_minor; return t; }
int64_t umi_accounting_journal_entry_total_credit(const UmiAccountingJournalEntry *value){ int64_t t=0; if(value==NULL)return 0; for(size_t i=0U;i<value->line_count;++i)t+=value->lines[i].credit_minor; return t; }
bool umi_accounting_journal_entry_balanced(const UmiAccountingJournalEntry *value){ return value!=NULL&&value->line_count>=2U&&umi_accounting_journal_entry_total_debit(value)==umi_accounting_journal_entry_total_credit(value); }
