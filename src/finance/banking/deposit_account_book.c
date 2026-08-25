/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/deposit_account_book.c
 *
 * PURPOSE:
 *   Implement maintain bounded deposit accounts for reusable banking services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_account_book.h"
#include <string.h>
void umi_banking_deposit_account_book_init(UmiBankingDepositAccountBook *registry) { if(registry!=NULL) memset(registry,0,sizeof *registry); }
const UmiBankingDepositAccount *umi_banking_deposit_account_book_find(const UmiBankingDepositAccountBook *registry,const char *id) {
    if(registry==NULL||id==NULL) return NULL;
    UmiFinancialId key;
    if(umi_financial_id_assign(&key,id)!=UMI_STATUS_OK) return NULL;
    for(size_t i=0U;i<registry->count;++i) {
        if(umi_financial_id_compare(&registry->items[i].id,&key)==0) return &registry->items[i];
    }
    return NULL;
}
UmiStatus umi_banking_deposit_account_book_add(UmiBankingDepositAccountBook *registry,const UmiBankingDepositAccount *item) {
    if(registry==NULL||item==NULL||!umi_banking_deposit_account_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    if(registry->count>=UMI_BANKING_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(umi_banking_deposit_account_book_find(registry,item->id.value)!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    registry->items[registry->count++]=*item; return UMI_STATUS_OK;
}
