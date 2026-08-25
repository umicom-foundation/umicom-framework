/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/bank_relationship.c
 *
 * PURPOSE:
 *   Implement represent customer-to-bank relationship ownership independent of presentation channels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_relationship.h"
#include <string.h>
UmiStatus umi_banking_bank_relationship_init(UmiBankingBankRelationship *value,
    const char *id,
    const char *customer_id,
    const char *relationship_manager,
    bool primary_relationship) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->relationship_manager,sizeof value->relationship_manager,relationship_manager);
    if(rc!=UMI_STATUS_OK)return rc;
    value->primary_relationship=primary_relationship;
    return umi_banking_bank_relationship_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_bank_relationship_valid(const UmiBankingBankRelationship *value) {
    return value!=NULL && (value->relationship_manager[0]!='\0');
}

bool umi_banking_bank_relationship_primary(const UmiBankingBankRelationship *value) {
    if(value==NULL) return (bool)0;
    return value->primary_relationship;
}
