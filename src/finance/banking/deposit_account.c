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
#include <stdint.h>
#include "umicom/finance/currency.h"
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
    UmiBankingDepositAccount candidate = {0};
    UmiStatus rc=umi_banking_id_assign(&candidate.id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&candidate.customer_id,customer_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&candidate.product_id,product_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_currency_from_code(currency_code,&candidate.currency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    candidate.ledger_minor=ledger_minor;
    candidate.available_minor=available_minor;
    candidate.status=status;
    if (!umi_banking_deposit_account_valid(&candidate)) return UMI_STATUS_INVALID_ARGUMENT;
    *value = candidate;
    return UMI_STATUS_OK;
}
/*
 * Check that banking deposit account satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_deposit_account_valid(const UmiBankingDepositAccount *value) {
    if (value == NULL || !umi_financial_id_is_valid(&value->id) ||
        !umi_financial_id_is_valid(&value->customer_id) ||
        !umi_financial_id_is_valid(&value->product_id) ||
        !umi_currency_valid(&value->currency) ||
        value->available_minor > value->ledger_minor ||
        value->status < UMI_BANKING_ACCOUNT_PENDING ||
        value->status > UMI_BANKING_ACCOUNT_CLOSED) return false;
    /* Negative balances remain supported. Only reject an unrepresentable
     * reservation, before subtracting two otherwise representable amounts. */
    return !(value->available_minor < 0 &&
        value->ledger_minor > INT64_MAX + value->available_minor);
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
    int64_t reserved = 0;
    (void)UmiBankingDepositAccountReserved(value, &reserved);
    return reserved;
}

/* A status-bearing alternative distinguishes a genuine zero reservation from
 * invalid data. The legacy helper above retains its zero-on-invalid convention. */
UmiStatus UmiBankingDepositAccountReserved(const UmiBankingDepositAccount *value,
    int64_t *outReserved)
{
    if (outReserved == NULL || !umi_banking_deposit_account_valid(value))
        return UMI_STATUS_INVALID_ARGUMENT;
    *outReserved = value->ledger_minor - value->available_minor;
    return UMI_STATUS_OK;
}
