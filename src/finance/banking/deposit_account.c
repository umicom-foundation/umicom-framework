/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/deposit_account.c
 *
 * PURPOSE:
 *   Implement represent customer deposit balances, currency and available funds.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_account.h"
#include <string.h>
UmiStatus umi_banking_deposit_account_init(UmiBankingDepositAccount *value,
    const char *id,
    const char *customer_id,
    const char *product_id,
    const char *currency_code,
    int64_t ledger_minor,
    int64_t available_minor,
    UmiBankingAccountStatus status) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->product_id,product_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_currency_from_code(currency_code,&value->currency);
    if(rc!=UMI_STATUS_OK)return rc;
    value->ledger_minor=ledger_minor;
    value->available_minor=available_minor;
    value->status=status;
    return umi_banking_deposit_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_deposit_account_valid(const UmiBankingDepositAccount *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && umi_financial_id_is_valid(&value->product_id) && value->available_minor<=value->ledger_minor && value->status>=UMI_BANKING_ACCOUNT_PENDING && value->status<=UMI_BANKING_ACCOUNT_CLOSED);
}

int64_t umi_banking_deposit_account_reserved_minor(const UmiBankingDepositAccount *value) {
    if(value==NULL) return (int64_t)0;
    return value->ledger_minor-value->available_minor;
}
