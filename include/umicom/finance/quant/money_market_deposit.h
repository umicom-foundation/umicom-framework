/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/money_market_deposit.h
 *
 * PURPOSE:
 *   Represent a simple-interest money-market deposit.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_MONEY_MARKET_DEPOSIT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_MONEY_MARKET_DEPOSIT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantMoneyMarketDeposit {
    double principal;
    double rate;
    int32_t days;
    int32_t day_basis;
} UmiQuantMoneyMarketDeposit;

/* Initialise and validate the money market deposit contract. */
UmiStatus umi_quant_money_market_deposit_init(UmiQuantMoneyMarketDeposit *record, double principal, double rate, int32_t days, int32_t day_basis);

/* Calculate simple accrued interest using the configured day basis. */
double umi_quant_money_market_deposit_interest(const UmiQuantMoneyMarketDeposit *record);

#ifdef __cplusplus
}
#endif

#endif
