/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/fixing.c
 *
 * PURPOSE:
 *   Implement financial fixing validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/fixing.h"

#include <math.h>
/* Initialize fixing. */ UmiStatus umi_fixing_init(UmiFixing *f,const UmiMarketDataKey *k,UmiFinancialDate d,double value,int64_t ts){if(f==NULL||k==NULL||!umi_market_data_key_is_valid(k)||!umi_financial_date_is_valid(d)||!isfinite(value)||ts<0)return UMI_STATUS_INVALID_ARGUMENT;f->key=*k;f->date=d;f->value=value;f->timestamp=ts;return UMI_STATUS_OK;}
/* Validate fixing. */ bool umi_fixing_is_valid(const UmiFixing *f){return f!=NULL&&umi_market_data_key_is_valid(&f->key)&&umi_financial_date_is_valid(f->date)&&isfinite(f->value)&&f->timestamp>=0;}
