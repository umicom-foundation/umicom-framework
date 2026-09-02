/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/deposit_account.c
 *
 * PURPOSE:
 *   Implement represent customer deposit balances, currency and available funds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_account.h"
#include <string.h>
/*
 * Initialise banking deposit account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_deposit_account_init(UmiBankingDepositAccount *value,
    const char *id,
    const char *customer_id,
    const char *product_id,
    const char *currency_code,
    int64_t ledger_minor,
    int64_t available_minor,
    UmiBankingAccountStatus status) {
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
    rc=umi_banking_id_assign(&value->product_id,product_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_currency_from_code(currency_code,&value->currency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->ledger_minor=ledger_minor;
    value->available_minor=available_minor;
    value->status=status;
    return umi_banking_deposit_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking deposit account satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_deposit_account_valid(const UmiBankingDepositAccount *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && umi_financial_id_is_valid(&value->product_id) && value->available_minor<=value->ledger_minor && value->status>=UMI_BANKING_ACCOUNT_PENDING && value->status<=UMI_BANKING_ACCOUNT_CLOSED);
}

/*
 * Provide the banking deposit account reserved minor operation used by this module and its
 * client applications.
 */
int64_t umi_banking_deposit_account_reserved_minor(const UmiBankingDepositAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return value->ledger_minor-value->available_minor;
}
