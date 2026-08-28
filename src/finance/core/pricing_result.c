/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/pricing_result.c
 *
 * PURPOSE:
 *   Implement provider-neutral pricing results.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/pricing_result.h"

#include <string.h>
/* Initialize monetary record. */ UmiStatus umi_pricing_result_init(UmiPricingResult *x,const char *id,UmiMoney amount,UmiFinancialDate date,uint32_t state){UmiStatus st;if(x==NULL||strlen(amount.currency.code)!=3U||!umi_financial_date_is_valid(date))return UMI_STATUS_INVALID_ARGUMENT;st=umi_financial_id_assign(&x->id,id);if(st!=UMI_STATUS_OK)return st;x->amount=amount;x->date=date;x->state=state;return UMI_STATUS_OK;}
/* Validate monetary record. */ bool umi_pricing_result_is_valid(const UmiPricingResult *x){return x!=NULL&&umi_financial_id_is_valid(&x->id)&&strlen(x->amount.currency.code)==3U&&umi_financial_date_is_valid(x->date);}
