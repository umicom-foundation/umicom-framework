/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/chart_of_accounts.c
 *
 * PURPOSE:
 *   Implement maintain a bounded chart of accounts with deterministic account lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/chart_of_accounts.h"
#include <string.h>
void umi_accounting_chart_of_accounts_init(UmiAccountingChartOfAccounts *registry) { if(registry!=NULL) memset(registry,0,sizeof *registry); }
const UmiAccountingLedgerAccount *umi_accounting_chart_of_accounts_find(const UmiAccountingChartOfAccounts *registry,const char *id) {
    if(registry==NULL||id==NULL) return NULL;
    UmiFinancialId key;
    if(umi_financial_id_assign(&key,id)!=UMI_STATUS_OK) return NULL;
    for(size_t i=0U;i<registry->count;++i) {
        if(umi_financial_id_compare(&registry->items[i].id,&key)==0) return &registry->items[i];
    }
    return NULL;
}
UmiStatus umi_accounting_chart_of_accounts_add(UmiAccountingChartOfAccounts *registry,const UmiAccountingLedgerAccount *item) {
    if(registry==NULL||item==NULL||!umi_accounting_ledger_account_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    if(registry->count>=UMI_ACCOUNTING_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(umi_accounting_chart_of_accounts_find(registry,item->id.value)!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    registry->items[registry->count++]=*item; return UMI_STATUS_OK;
}
