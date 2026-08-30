/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/quote.h
 *
 * PURPOSE:
 *   Represent timestamped market quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_QUOTE_H
#define UMICOM_FINANCE_CORE_QUOTE_H

#include "umicom/finance/core/price.h"
#include "umicom/finance/core/market_data_key.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiQuote { UmiMarketDataKey key; UmiQuoteSide side; UmiFinancialPrice price; int64_t timestamp; } UmiQuote;
/* Initialize quote. */ UmiStatus umi_quote_init(UmiQuote *q,const UmiMarketDataKey *k,UmiQuoteSide side,UmiFinancialPrice p,int64_t ts);
/* Validate quote. */ bool umi_quote_is_valid(const UmiQuote *q);
#ifdef __cplusplus
}
#endif

#endif
