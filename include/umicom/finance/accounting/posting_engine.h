/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/posting_engine.h
 *
 * PURPOSE:
 *   Transform canonical accounting events and active posting rules into balanced journal entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_POSTING_ENGINE_H
#define UMICOM_FINANCE_ACCOUNTING_POSTING_ENGINE_H
#include "umicom/finance/accounting/accounting_event.h"
#include "umicom/finance/accounting/posting_rule.h"
#include "umicom/finance/accounting/journal_entry.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting posting engine data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingPostingEngine { size_t generated_entries; size_t rejected_events; } UmiAccountingPostingEngine;
/**
 * Initialise accounting posting engine from caller-provided values so later operations
 * receive a known state.
 */
void umi_accounting_posting_engine_init(UmiAccountingPostingEngine *value);
/**
 * Provide the accounting posting engine generate operation used by this module and its
 * client applications.
 */
UmiStatus umi_accounting_posting_engine_generate(UmiAccountingPostingEngine *value,const UmiAccountingAccountingEvent *event,const UmiAccountingPostingRule *rule,const char *journal_id,UmiAccountingJournalEntry *out_entry);
/**
 * Check that accounting posting engine satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_posting_engine_valid(const UmiAccountingPostingEngine *value);
#ifdef __cplusplus
}
#endif
#endif
