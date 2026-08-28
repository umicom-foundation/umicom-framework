/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/valuation.h
 *
 * PURPOSE:
 *   Represent immutable trade valuation records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_VALUATION_H
#define UMICOM_FINANCE_CORE_VALUATION_H

#include "umicom/finance/core/business_date.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiValuation { UmiFinancialId id; UmiMoney amount; UmiFinancialDate date; uint32_t state; } UmiValuation;
/* Initialize monetary record. */ UmiStatus umi_valuation_init(UmiValuation *x,const char *id,UmiMoney amount,UmiFinancialDate date,uint32_t state);
/* Validate monetary record. */ bool umi_valuation_is_valid(const UmiValuation *x);
#ifdef __cplusplus
}
#endif

#endif
