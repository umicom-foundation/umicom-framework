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
/*
 * Initialise accounting posting engine from caller-provided values so later operations
 * receive a known state.
 */
void umi_accounting_posting_engine_init(UmiAccountingPostingEngine *value){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL)memset(value,0,sizeof *value); }
/*
 * Check that accounting posting engine satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_posting_engine_valid(const UmiAccountingPostingEngine *value){ return value!=NULL; }
/*
 * Provide the accounting posting engine generate operation used by this module and its
 * client applications.
 */
UmiStatus umi_accounting_posting_engine_generate(UmiAccountingPostingEngine *value,const UmiAccountingAccountingEvent *event,const UmiAccountingPostingRule *rule,const char *journal_id,UmiAccountingJournalEntry *out_entry){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||event==NULL||rule==NULL||out_entry==NULL||journal_id==NULL||!umi_accounting_accounting_event_valid(event)||!umi_accounting_posting_rule_valid(rule)||!rule->active){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL)++value->rejected_events; return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(event->event_type,rule->event_type)!=0){ ++value->rejected_events; return UMI_STATUS_NOT_FOUND; } int64_t amount=umi_accounting_abs_i64(event->amount_minor); UmiStatus s=umi_accounting_journal_entry_init(out_entry,journal_id,event->accounting_date); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; UmiAccountingJournalLine debit,credit; s=umi_accounting_journal_line_init(&debit,"generated-debit",rule->debit_account_id.value,amount,0); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; s=umi_accounting_journal_line_init(&credit,"generated-credit",rule->credit_account_id.value,0,amount); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_accounting_journal_entry_add_line(out_entry,&debit)!=UMI_STATUS_OK||umi_accounting_journal_entry_add_line(out_entry,&credit)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR; ++value->generated_entries; return UMI_STATUS_OK; }
