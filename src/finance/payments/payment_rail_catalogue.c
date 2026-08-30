/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_rail_catalogue.c
 *
 * PURPOSE:
 *   Implement catalogue payment rails with deterministic identifier lookup and duplicate prevention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_rail_catalogue.h"
#include <string.h>
void umi_payments_payment_rail_catalogue_init(UmiPaymentsPaymentRailCatalogue *registry) { if(registry!=NULL) memset(registry,0,sizeof *registry); }
const UmiPaymentsPaymentRail *umi_payments_payment_rail_catalogue_find(const UmiPaymentsPaymentRailCatalogue *registry,const char *id) {
    if(registry==NULL||id==NULL) return NULL;
    UmiFinancialId key;
    if(umi_financial_id_assign(&key,id)!=UMI_STATUS_OK) return NULL;
    for(size_t i=0U;i<registry->count;++i) {
        if(umi_financial_id_compare(&registry->items[i].id,&key)==0) return &registry->items[i];
    }
    return NULL;
}
UmiStatus umi_payments_payment_rail_catalogue_add(UmiPaymentsPaymentRailCatalogue *registry,const UmiPaymentsPaymentRail *item) {
    if(registry==NULL||item==NULL||!umi_payments_payment_rail_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    if(registry->count>=UMI_PAYMENTS_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(umi_payments_payment_rail_catalogue_find(registry,item->id.value)!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    registry->items[registry->count++]=*item; return UMI_STATUS_OK;
}
