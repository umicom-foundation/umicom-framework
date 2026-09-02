/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/deposit_transaction.h
 *
 * PURPOSE:
 *   Represent booked customer deposit movements with booking/value dates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_DEPOSIT_TRANSACTION_H
#define UMICOM_FINANCE_BANKING_DEPOSIT_TRANSACTION_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking deposit transaction data shared with callers of this public
 * contract.
 */
typedef struct UmiBankingDepositTransaction {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t amount_minor;
    UmiFinancialDate booking_date;
    UmiFinancialDate value_date;
} UmiBankingDepositTransaction;
/**
 * Initialise banking deposit transaction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_deposit_transaction_init(UmiBankingDepositTransaction *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    UmiFinancialDate booking_date,
    UmiFinancialDate value_date);
/**
 * Check that banking deposit transaction satisfies its contract before another service
 * relies on it.
 */
bool umi_banking_deposit_transaction_valid(const UmiBankingDepositTransaction *value);
/**
 * Provide the banking deposit transaction credit operation used by this module and its
 * client applications.
 */
bool umi_banking_deposit_transaction_credit(const UmiBankingDepositTransaction *value);
#ifdef __cplusplus
}
#endif
#endif
