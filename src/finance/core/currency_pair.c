/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/currency_pair.c
 *
 * PURPOSE:
 *   Implement ordered currency-pair construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/currency_pair.h"

#include <stdio.h>
#include <string.h>
/* Initialize an ordered pair. */
UmiStatus umi_currency_pair_init(UmiCurrencyPair *p,UmiCurrency base,UmiCurrency quote){int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||strlen(base.code)!=3U||strlen(quote.code)!=3U||strcmp(base.code,quote.code)==0)return UMI_STATUS_INVALID_ARGUMENT;p->base=base;p->quote=quote;n=snprintf(p->symbol,sizeof p->symbol,"%.3s%.3s",base.code,quote.code);return n==6?UMI_STATUS_OK:UMI_STATUS_INTERNAL_ERROR;}
/* Invert an ordered pair. */
UmiStatus umi_currency_pair_invert(const UmiCurrencyPair *p,UmiCurrencyPair *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_currency_pair_init(out,p->quote,p->base);}
