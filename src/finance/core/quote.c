/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/quote.c
 *
 * PURPOSE:
 *   Implement timestamped quote validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/quote.h"

/* Initialize quote. */ UmiStatus umi_quote_init(UmiQuote *q,const UmiMarketDataKey *k,UmiQuoteSide side,UmiFinancialPrice p,int64_t ts){if(q==NULL||k==NULL||!umi_market_data_key_is_valid(k)||side>UMI_QUOTE_LAST||!umi_price_is_valid(&p)||ts<0)return UMI_STATUS_INVALID_ARGUMENT;q->key=*k;q->side=side;q->price=p;q->timestamp=ts;return UMI_STATUS_OK;}
/* Validate quote. */ bool umi_quote_is_valid(const UmiQuote *q){return q!=NULL&&umi_market_data_key_is_valid(&q->key)&&q->side<=UMI_QUOTE_LAST&&umi_price_is_valid(&q->price)&&q->timestamp>=0;}
