/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/chart_of_accounts.h
 *
 * PURPOSE:
 *   Maintain a bounded chart of accounts with deterministic account lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_CHART_OF_ACCOUNTS_H
#define UMICOM_FINANCE_ACCOUNTING_CHART_OF_ACCOUNTS_H
#include "umicom/finance/accounting/ledger_account.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingChartOfAccounts { UmiAccountingLedgerAccount items[UMI_ACCOUNTING_MAX_ITEMS]; size_t count; } UmiAccountingChartOfAccounts;
void umi_accounting_chart_of_accounts_init(UmiAccountingChartOfAccounts *registry);
UmiStatus umi_accounting_chart_of_accounts_add(UmiAccountingChartOfAccounts *registry,const UmiAccountingLedgerAccount *item);
const UmiAccountingLedgerAccount *umi_accounting_chart_of_accounts_find(const UmiAccountingChartOfAccounts *registry,const char *id);
#ifdef __cplusplus
}
#endif
#endif
