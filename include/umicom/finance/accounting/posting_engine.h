/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/posting_engine.h
 *
 * PURPOSE:
 *   Transform canonical accounting events and active posting rules into balanced journal entries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_POSTING_ENGINE_H
#define UMICOM_FINANCE_ACCOUNTING_POSTING_ENGINE_H
#include "umicom/finance/accounting/accounting_event.h"
#include "umicom/finance/accounting/posting_rule.h"
#include "umicom/finance/accounting/journal_entry.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingPostingEngine { size_t generated_entries; size_t rejected_events; } UmiAccountingPostingEngine;
void umi_accounting_posting_engine_init(UmiAccountingPostingEngine *value);
UmiStatus umi_accounting_posting_engine_generate(UmiAccountingPostingEngine *value,const UmiAccountingAccountingEvent *event,const UmiAccountingPostingRule *rule,const char *journal_id,UmiAccountingJournalEntry *out_entry);
bool umi_accounting_posting_engine_valid(const UmiAccountingPostingEngine *value);
#ifdef __cplusplus
}
#endif
#endif
