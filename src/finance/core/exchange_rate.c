/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/exchange_rate.c
 *
 * PURPOSE:
 *   Implement positive finite FX rate validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/exchange_rate.h"

#include <math.h>
/* Initialize rate. */ UmiStatus umi_exchange_rate_init(UmiExchangeRate *r,const UmiCurrencyPair *p,double rate,int64_t ts){if(r==NULL||p==NULL||!isfinite(rate)||rate<=0.0||ts<0)return UMI_STATUS_INVALID_ARGUMENT;r->pair=*p;r->rate=rate;r->timestamp=ts;return UMI_STATUS_OK;}
/* Validate rate. */ bool umi_exchange_rate_is_valid(const UmiExchangeRate *r){return r!=NULL&&r->pair.symbol[0]!='\0'&&isfinite(r->rate)&&r->rate>0.0&&r->timestamp>=0;}
