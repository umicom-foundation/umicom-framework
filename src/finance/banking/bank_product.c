/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/bank_product.c
 *
 * PURPOSE:
 *   Implement describe reusable banking product templates independent of channel applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_product.h"
#include <string.h>
UmiStatus umi_banking_bank_product_init(UmiBankingBankProduct *value,
    const char *id,
    const char *name,
    UmiBankingProductKind kind,
    bool active) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    if(rc!=UMI_STATUS_OK)return rc;
    value->kind=kind;
    value->active=active;
    return umi_banking_bank_product_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_bank_product_valid(const UmiBankingBankProduct *value) {
    return value!=NULL && (value->name[0]!='\0' && value->kind>=UMI_BANKING_PRODUCT_DEPOSIT && value->kind<=UMI_BANKING_PRODUCT_OVERDRAFT);
}

bool umi_banking_bank_product_available(const UmiBankingBankProduct *value) {
    if(value==NULL) return (bool)0;
    return value->active;
}
