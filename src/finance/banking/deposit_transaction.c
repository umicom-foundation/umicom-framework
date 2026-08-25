/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/deposit_transaction.c
 *
 * PURPOSE:
 *   Implement represent booked customer deposit movements with booking/value dates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_transaction.h"
#include <string.h>
UmiStatus umi_banking_deposit_transaction_init(UmiBankingDepositTransaction *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    UmiFinancialDate booking_date,
    UmiFinancialDate value_date) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->account_id,account_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->amount_minor=amount_minor;
    value->booking_date=booking_date;
    value->value_date=value_date;
    return umi_banking_deposit_transaction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_deposit_transaction_valid(const UmiBankingDepositTransaction *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->account_id) && value->amount_minor!=0 && umi_financial_date_is_valid(value->booking_date) && umi_financial_date_is_valid(value->value_date));
}

bool umi_banking_deposit_transaction_credit(const UmiBankingDepositTransaction *value) {
    if(value==NULL) return (bool)0;
    return value->amount_minor>0;
}
