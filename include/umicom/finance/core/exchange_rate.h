/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/exchange_rate.h
 *
 * PURPOSE:
 *   Represent positive finite FX rates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_EXCHANGE_RATE_H
#define UMICOM_FINANCE_CORE_EXCHANGE_RATE_H

#include "umicom/finance/core/currency_pair.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the exchange rate data shared with callers of this public contract.
 */
typedef struct UmiExchangeRate { UmiCurrencyPair pair; double rate; int64_t timestamp; } UmiExchangeRate;
/* Initialize rate. */ UmiStatus umi_exchange_rate_init(UmiExchangeRate *r,const UmiCurrencyPair *p,double rate,int64_t ts);
/* Validate rate. */ bool umi_exchange_rate_is_valid(const UmiExchangeRate *r);
#ifdef __cplusplus
}
#endif

#endif
