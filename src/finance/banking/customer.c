/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/customer.c
 *
 * PURPOSE:
 *   Implement represent a canonical banking customer linked to the shared legal-entity model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer.h"
#include <string.h>
UmiStatus umi_banking_customer_init(UmiBankingCustomer *value,
    const char *id,
    const char *legal_entity_id,
    const char *display_name,
    UmiBankingCustomerStatus status) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->legal_entity_id,legal_entity_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->display_name,sizeof value->display_name,display_name);
    if(rc!=UMI_STATUS_OK)return rc;
    value->status=status;
    return umi_banking_customer_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_customer_valid(const UmiBankingCustomer *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->id) && umi_financial_id_is_valid(&value->legal_entity_id) && value->display_name[0]!='\0' && value->status>=UMI_BANKING_CUSTOMER_PROSPECT && value->status<=UMI_BANKING_CUSTOMER_CLOSED);
}

bool umi_banking_customer_operational(const UmiBankingCustomer *value) {
    if(value==NULL) return (bool)0;
    return value->status==UMI_BANKING_CUSTOMER_ACTIVE;
}
