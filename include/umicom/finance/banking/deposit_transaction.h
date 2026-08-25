/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/deposit_transaction.h
 *
 * PURPOSE:
 *   Represent booked customer deposit movements with booking/value dates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_DEPOSIT_TRANSACTION_H
#define UMICOM_FINANCE_BANKING_DEPOSIT_TRANSACTION_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingDepositTransaction {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t amount_minor;
    UmiFinancialDate booking_date;
    UmiFinancialDate value_date;
} UmiBankingDepositTransaction;
UmiStatus umi_banking_deposit_transaction_init(UmiBankingDepositTransaction *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    UmiFinancialDate booking_date,
    UmiFinancialDate value_date);
bool umi_banking_deposit_transaction_valid(const UmiBankingDepositTransaction *value);
bool umi_banking_deposit_transaction_credit(const UmiBankingDepositTransaction *value);
#ifdef __cplusplus
}
#endif
#endif
