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
/**
 * Represent the accounting chart of accounts data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingChartOfAccounts { UmiAccountingLedgerAccount items[UMI_ACCOUNTING_MAX_ITEMS]; size_t count; } UmiAccountingChartOfAccounts;
/**
 * Initialise accounting chart of accounts from caller-provided values so later operations
 * receive a known state.
 */
void umi_accounting_chart_of_accounts_init(UmiAccountingChartOfAccounts *registry);
/**
 * Add accounting chart of accounts only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_accounting_chart_of_accounts_add(UmiAccountingChartOfAccounts *registry,const UmiAccountingLedgerAccount *item);
/**
 * Find accounting chart of accounts while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiAccountingLedgerAccount *umi_accounting_chart_of_accounts_find(const UmiAccountingChartOfAccounts *registry,const char *id);
#ifdef __cplusplus
}
#endif
#endif
