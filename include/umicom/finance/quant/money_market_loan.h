/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/money_market_loan.h
 *
 * PURPOSE:
 *   Represent a simple-interest money-market loan.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_MONEY_MARKET_LOAN_H
#define INCLUDE_UMICOM_FINANCE_QUANT_MONEY_MARKET_LOAN_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant money market loan data shared with callers of this public contract.
 */
typedef struct UmiQuantMoneyMarketLoan {
    double principal;
    double rate;
    int32_t days;
    int32_t day_basis;
} UmiQuantMoneyMarketLoan;

/* Initialise and validate the money market loan contract. */
UmiStatus umi_quant_money_market_loan_init(UmiQuantMoneyMarketLoan *record, double principal, double rate, int32_t days, int32_t day_basis);

/* Calculate borrower interest using simple money-market accrual. */
double umi_quant_money_market_loan_interest(const UmiQuantMoneyMarketLoan *record);

#ifdef __cplusplus
}
#endif

#endif
