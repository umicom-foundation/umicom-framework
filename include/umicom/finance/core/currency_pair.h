/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/currency_pair.h
 *
 * PURPOSE:
 *   Represent ordered base/quote currency pairs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_CURRENCY_PAIR_H
#define UMICOM_FINANCE_CORE_CURRENCY_PAIR_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCurrencyPair { UmiCurrency base; UmiCurrency quote; char symbol[8]; } UmiCurrencyPair;
/* Initialize an ordered pair. */ UmiStatus umi_currency_pair_init(UmiCurrencyPair *p,UmiCurrency base,UmiCurrency quote);
/* Invert an ordered pair. */ UmiStatus umi_currency_pair_invert(const UmiCurrencyPair *p,UmiCurrencyPair *out);
#ifdef __cplusplus
}
#endif

#endif
