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
/*
 * Initialise payments payment rail catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_payments_payment_rail_catalogue_init(UmiPaymentsPaymentRailCatalogue *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry!=NULL) memset(registry,0,sizeof *registry); }
/*
 * Find payments payment rail catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiPaymentsPaymentRail *umi_payments_payment_rail_catalogue_find(const UmiPaymentsPaymentRailCatalogue *registry,const char *id) {
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
 * Add payments payment rail catalogue only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_payments_payment_rail_catalogue_add(UmiPaymentsPaymentRailCatalogue *registry,const UmiPaymentsPaymentRail *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(registry==NULL||item==NULL||!umi_payments_payment_rail_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(registry->count>=UMI_PAYMENTS_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(umi_payments_payment_rail_catalogue_find(registry,item->id.value)!=NULL) return UMI_STATUS_ALREADY_EXISTS;
    registry->items[registry->count++]=*item; return UMI_STATUS_OK;
}
