/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/price.h
 *
 * PURPOSE:
 *   Represent finite non-negative financial prices.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRICE_H
#define UMICOM_FINANCE_CORE_PRICE_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinancialPrice { double value; uint8_t scale; } UmiFinancialPrice;
/* Initialize price. */ UmiStatus umi_price_init(UmiFinancialPrice *p,double value,uint8_t scale);
/* Validate price. */ bool umi_price_is_valid(const UmiFinancialPrice *p);
#ifdef __cplusplus
}
#endif

#endif
