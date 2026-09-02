/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/bank_relationship.c
 *
 * PURPOSE:
 *   Implement represent customer-to-bank relationship ownership independent of presentation channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_relationship.h"
#include <string.h>
/*
 * Initialise banking bank relationship from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_bank_relationship_init(UmiBankingBankRelationship *value,
    const char *id,
    const char *customer_id,
    const char *relationship_manager,
    bool primary_relationship) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->relationship_manager,sizeof value->relationship_manager,relationship_manager);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->primary_relationship=primary_relationship;
    return umi_banking_bank_relationship_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking bank relationship satisfies its contract before another service
 * relies on it.
 */
bool umi_banking_bank_relationship_valid(const UmiBankingBankRelationship *value) {
    return value!=NULL && (value->relationship_manager[0]!='\0');
}

/*
 * Provide the banking bank relationship primary operation used by this module and its
 * client applications.
 */
bool umi_banking_bank_relationship_primary(const UmiBankingBankRelationship *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->primary_relationship;
}
