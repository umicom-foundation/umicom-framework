/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/bank_product.c
 *
 * PURPOSE:
 *   Implement describe reusable banking product templates independent of channel applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_product.h"
#include <string.h>
/*
 * Initialise banking bank product from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_bank_product_init(UmiBankingBankProduct *value,
    const char *id,
    const char *name,
    UmiBankingProductKind kind,
    bool active) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->kind=kind;
    value->active=active;
    return umi_banking_bank_product_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking bank product satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_bank_product_valid(const UmiBankingBankProduct *value) {
    return value!=NULL && (value->name[0]!='\0' && value->kind>=UMI_BANKING_PRODUCT_DEPOSIT && value->kind<=UMI_BANKING_PRODUCT_OVERDRAFT);
}

/*
 * Provide the banking bank product available operation used by this module and its client
 * applications.
 */
bool umi_banking_bank_product_available(const UmiBankingBankProduct *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->active;
}
