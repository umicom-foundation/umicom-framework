/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/chart_of_accounts.c
 *
 * PURPOSE:
 *   Implement maintain a bounded chart of accounts with deterministic account lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/chart_of_accounts.h"
#include <string.h>
/*
 * Initialise accounting chart of accounts from caller-provided values so later operations
 * receive a known state.
 */
void umi_accounting_chart_of_accounts_init(UmiAccountingChartOfAccounts *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry!=NULL) memset(registry,0,sizeof *registry); }
/*
 * Find accounting chart of accounts while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiAccountingLedgerAccount *umi_accounting_chart_of_accounts_find(const UmiAccountingChartOfAccounts *registry,const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(registry==NULL||id==NULL) return NULL;
    UmiFinancialId key;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_financial_id_assign(&key,id)!=UMI_STATUS_OK) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(size_t i=0U;i<registry->count;++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(umi_financial_id_compare(&registry->items[i].id,&key)==0) return &registry->items[i];
    }
    return NULL;
}
/*
 * Add accounting chart of accounts only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_accounting_chart_of_accounts_add(UmiAccountingChartOfAccounts *registry,const UmiAccountingLedgerAccount *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(registry==NULL||item==NULL||!umi_accounting_ledger_account_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(registry->count>=UMI_ACCOUNTING_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(umi_accounting_chart_of_accounts_find(registry,item->id.value)!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    registry->items[registry->count++]=*item; return UMI_STATUS_OK;
}
