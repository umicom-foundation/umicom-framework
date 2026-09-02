/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/deposit_transaction.c
 *
 * PURPOSE:
 *   Implement represent booked customer deposit movements with booking/value dates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_transaction.h"
#include <string.h>
/*
 * Initialise banking deposit transaction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_deposit_transaction_init(UmiBankingDepositTransaction *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    UmiFinancialDate booking_date,
    UmiFinancialDate value_date) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->account_id,account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->amount_minor=amount_minor;
    value->booking_date=booking_date;
    value->value_date=value_date;
    return umi_banking_deposit_transaction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking deposit transaction satisfies its contract before another service
 * relies on it.
 */
bool umi_banking_deposit_transaction_valid(const UmiBankingDepositTransaction *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->account_id) && value->amount_minor!=0 && umi_financial_date_is_valid(value->booking_date) && umi_financial_date_is_valid(value->value_date));
}

/*
 * Provide the banking deposit transaction credit operation used by this module and its
 * client applications.
 */
bool umi_banking_deposit_transaction_credit(const UmiBankingDepositTransaction *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->amount_minor>0;
}
