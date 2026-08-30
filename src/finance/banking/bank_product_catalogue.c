/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/bank_product_catalogue.c
 *
 * PURPOSE:
 *   Implement catalogue reusable banking product templates with stable lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_product_catalogue.h"
#include <string.h>
void umi_banking_bank_product_catalogue_init(UmiBankingBankProductCatalogue *registry) { if(registry!=NULL) memset(registry,0,sizeof *registry); }
const UmiBankingBankProduct *umi_banking_bank_product_catalogue_find(const UmiBankingBankProductCatalogue *registry,const char *id) {
    if(registry==NULL||id==NULL) return NULL;
    UmiFinancialId key;
    if(umi_financial_id_assign(&key,id)!=UMI_STATUS_OK) return NULL;
    for(size_t i=0U;i<registry->count;++i) {
        if(umi_financial_id_compare(&registry->items[i].id,&key)==0) return &registry->items[i];
    }
    return NULL;
}
UmiStatus umi_banking_bank_product_catalogue_add(UmiBankingBankProductCatalogue *registry,const UmiBankingBankProduct *item) {
    if(registry==NULL||item==NULL||!umi_banking_bank_product_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    if(registry->count>=UMI_BANKING_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(umi_banking_bank_product_catalogue_find(registry,item->id.value)!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    registry->items[registry->count++]=*item; return UMI_STATUS_OK;
}
