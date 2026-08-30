/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/posting_engine.c
 *
 * PURPOSE:
 *   Implement deterministic two-sided journal generation from accounting events and posting rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_engine.h"
#include <string.h>
void umi_accounting_posting_engine_init(UmiAccountingPostingEngine *value){ if(value!=NULL)memset(value,0,sizeof *value); }
bool umi_accounting_posting_engine_valid(const UmiAccountingPostingEngine *value){ return value!=NULL; }
UmiStatus umi_accounting_posting_engine_generate(UmiAccountingPostingEngine *value,const UmiAccountingAccountingEvent *event,const UmiAccountingPostingRule *rule,const char *journal_id,UmiAccountingJournalEntry *out_entry){ if(value==NULL||event==NULL||rule==NULL||out_entry==NULL||journal_id==NULL||!umi_accounting_accounting_event_valid(event)||!umi_accounting_posting_rule_valid(rule)||!rule->active){ if(value!=NULL)++value->rejected_events; return UMI_STATUS_INVALID_ARGUMENT; } if(strcmp(event->event_type,rule->event_type)!=0){ ++value->rejected_events; return UMI_STATUS_NOT_FOUND; } int64_t amount=umi_accounting_abs_i64(event->amount_minor); UmiStatus s=umi_accounting_journal_entry_init(out_entry,journal_id,event->accounting_date); if(s!=UMI_STATUS_OK)return s; UmiAccountingJournalLine debit,credit; s=umi_accounting_journal_line_init(&debit,"generated-debit",rule->debit_account_id.value,amount,0); if(s!=UMI_STATUS_OK)return s; s=umi_accounting_journal_line_init(&credit,"generated-credit",rule->credit_account_id.value,0,amount); if(s!=UMI_STATUS_OK)return s; if(umi_accounting_journal_entry_add_line(out_entry,&debit)!=UMI_STATUS_OK||umi_accounting_journal_entry_add_line(out_entry,&credit)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR; ++value->generated_entries; return UMI_STATUS_OK; }
