/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/posting_rule.h
 *
 * PURPOSE:
 *   Map canonical accounting event types to debit and credit ledger accounts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_POSTING_RULE_H
#define UMICOM_FINANCE_ACCOUNTING_POSTING_RULE_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingPostingRule {
    UmiFinancialId id;
    char event_type[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    UmiFinancialId debit_account_id;
    UmiFinancialId credit_account_id;
    bool active;
} UmiAccountingPostingRule;
UmiStatus umi_accounting_posting_rule_init(UmiAccountingPostingRule *value,
    const char *id,
    const char *event_type,
    const char *debit_account_id,
    const char *credit_account_id,
    bool active);
bool umi_accounting_posting_rule_valid(const UmiAccountingPostingRule *value);
bool umi_accounting_posting_rule_usable(const UmiAccountingPostingRule *value);
#ifdef __cplusplus
}
#endif
#endif
