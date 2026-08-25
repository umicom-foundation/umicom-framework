/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/customer_registry.c
 *
 * PURPOSE:
 *   Implement maintain bounded canonical banking customers with deterministic identifier lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer_registry.h"
#include <string.h>
void umi_banking_customer_registry_init(UmiBankingCustomerRegistry *registry) { if(registry!=NULL) memset(registry,0,sizeof *registry); }
const UmiBankingCustomer *umi_banking_customer_registry_find(const UmiBankingCustomerRegistry *registry,const char *id) {
    if(registry==NULL||id==NULL) return NULL;
    UmiFinancialId key;
    if(umi_financial_id_assign(&key,id)!=UMI_STATUS_OK) return NULL;
    for(size_t i=0U;i<registry->count;++i) {
        if(umi_financial_id_compare(&registry->items[i].id,&key)==0) return &registry->items[i];
    }
    return NULL;
}
UmiStatus umi_banking_customer_registry_add(UmiBankingCustomerRegistry *registry,const UmiBankingCustomer *item) {
    if(registry==NULL||item==NULL||!umi_banking_customer_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    if(registry->count>=UMI_BANKING_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(umi_banking_customer_registry_find(registry,item->id.value)!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    registry->items[registry->count++]=*item; return UMI_STATUS_OK;
}
