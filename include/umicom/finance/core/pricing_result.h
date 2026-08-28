/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/pricing_result.h
 *
 * PURPOSE:
 *   Represent provider-neutral pricing results.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRICING_RESULT_H
#define UMICOM_FINANCE_CORE_PRICING_RESULT_H

#include "umicom/finance/core/business_date.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPricingResult { UmiFinancialId id; UmiMoney amount; UmiFinancialDate date; uint32_t state; } UmiPricingResult;
/* Initialize monetary record. */ UmiStatus umi_pricing_result_init(UmiPricingResult *x,const char *id,UmiMoney amount,UmiFinancialDate date,uint32_t state);
/* Validate monetary record. */ bool umi_pricing_result_is_valid(const UmiPricingResult *x);
#ifdef __cplusplus
}
#endif

#endif
